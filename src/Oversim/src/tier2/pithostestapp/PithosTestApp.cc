//
// Copyright (C) 2007 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @file PithosTestApp.cc
 * @author John Gilmore
 * @author Ingmar Baumgart
 */

#include <IPAddressResolver.h>
#include <GlobalNodeListAccess.h>
#include <GlobalStatisticsAccess.h>
#include <UnderlayConfiguratorAccess.h>
#include <RpcMacros.h>

#include <GlobalDhtTestMap.h>

#include "PithosTestApp.h"

Define_Module(PithosTestApp);

using namespace std;

PithosTestApp::~PithosTestApp()
{
    cancelAndDelete(pithostestput_timer);
    cancelAndDelete(pithostestget_timer);
    cancelAndDelete(pithostestmod_timer);
}

PithosTestApp::PithosTestApp()
{
    pithostestput_timer = NULL;
    pithostestget_timer = NULL;
    pithostestmod_timer = NULL;
}

void PithosTestApp::initializeApp(int stage)
{
    if (stage != MIN_STAGE_APP)
        return;

    // fetch parameters
    debugOutput = par("debugOutput");
    activeNetwInitPhase = par("activeNetwInitPhase");

    objectSize_av = par("avObjectSize");
	wait_time = par("wait_time");
	generationTime = par("generation_time");
    mean = par("testInterval");
    deviation = mean / 10;

    ttl = par("testTtl");

    globalNodeList = GlobalNodeListAccess().get();
    underlayConfigurator = UnderlayConfiguratorAccess().get();
    globalStatistics = GlobalStatisticsAccess().get();

    globalPithosTestMap = dynamic_cast<GlobalPithosTestMap*>(simulation.getModuleByPath("globalObserver.globalFunctions[0].function"));

    if (globalPithosTestMap == NULL) {
        throw cRuntimeError("PithosTestApp::initializeApp(): GlobalDhtTestMap module not found!");
    }

    // statistics
    numSent = 0;
    numGetSent = 0;
    numGetError = 0;
    numGetSuccess = 0;
    numPutSent = 0;
    numPutError = 0;
    numPutSuccess = 0;

    //initRpcs();
    WATCH(numSent);
    WATCH(numGetSent);
    WATCH(numGetError);
    WATCH(numGetSuccess);
    WATCH(numPutSent);
    WATCH(numPutError);
    WATCH(numPutSuccess);

    nodeIsLeavingSoon = false;

    // initiate test message transmission
    pithostestput_timer = new cMessage("pithostest_put_timer");
    pithostestget_timer = new cMessage("pithostest_get_timer");
    pithostestmod_timer = new cMessage("pithostest_mod_timer");
}

void PithosTestApp::handleRpcResponse(BaseResponseMessage* msg, const RpcState& state, simtime_t rtt)
{
    RPC_SWITCH_START(msg)
    RPC_ON_RESPONSE( RootObjectPutCAPI ) {
        handlePutResponse(_RootObjectPutCAPIResponse, check_and_cast<PithosStatsContext*>(state.getContext()));
        EV << "[PithosTestApp::handleRpcResponse()]\n"
           << "    Pithos Put RPC Response received: id=" << state.getId()
           << " msg=" << *_RootObjectPutCAPIResponse << " rtt=" << rtt
           << endl;
        break;
    }
    RPC_ON_RESPONSE(RootObjectGetCAPI)
    {
        handleGetResponse(_RootObjectGetCAPIResponse, check_and_cast<PithosStatsContext*>(state.getContext()));
        EV << "[PithosTestApp::handleRpcResponse()]\n"
           << "    Pithos Get RPC Response received: id=" << state.getId()
           << " msg=" << *_RootObjectGetCAPIResponse << " rtt=" << rtt
           << endl;
        break;
    }
    RPC_SWITCH_END()
}

void PithosTestApp::handlePutResponse(RootObjectPutCAPIResponse* msg, PithosStatsContext* context)
{
    if (context->measurementPhase == false) {
        // don't count response, if the request was not sent
        // in the measurement phase
        delete context;
        return;
    }

    //Only add the object to the global test map if it was successful
    if (msg->getIsSuccess())
    {
        GameObject object(context->go);

        if (msg->getGroupAddress().isUnspecified())
        	error("Received unspecified group address for stored object from Pithos.");

        //This allows us to perform group specific queries (Changing the group address does not change the object hash)
        object.setGroupAddress(msg->getGroupAddress());

        //globalPithosTestMap->insertEntry(context->go.getHash(), context->go);
        globalPithosTestMap->insertEntry(object.getHash(), object);

        EV << "Storing pithos entry: " << object.getHash() << endl;

        RECORD_STATS(numPutSuccess++);
        RECORD_STATS(globalStatistics->addStdDev("PithosTestApp: PUT Latency (s)", SIMTIME_DBL(simTime() - context->requestTime)));
    } else {
        RECORD_STATS(numPutError++);
    }

    delete context;
}

void PithosTestApp::handleGetResponse(RootObjectGetCAPIResponse* msg, PithosStatsContext* context)
{
    if (context->measurementPhase == false) {
        // don't count response, if the request was not sent
        // in the measurement phase
        delete context;
        return;
    }

    RECORD_STATS(globalStatistics->addStdDev("PithosTestApp: GET Latency (s)", SIMTIME_DBL(simTime() - context->requestTime)));

    if (!(msg->getIsSuccess())) {
        //cout << "PithosTestApp: success == false" << endl;
        RECORD_STATS(numGetError++);
        delete context;
        return;
    }

    const GameObject *entry = globalPithosTestMap->findEntry(context->key);

    if (entry == NULL) {
        //unexpected key
        RECORD_STATS(numGetError++);
        //cout << "PithosTestApp: unexpected key" << endl;
        delete context;
        return;
    }

    if (simTime() > entry->getCreationTime() + entry->getTTL()) {
        //this key doesn't exist anymore in Pithos, delete it in our hashtable

    	globalPithosTestMap->eraseEntry(context->key);
        delete context;

        if (msg->getResult() == GameObject::UNSPECIFIED_OBJECT) {
            RECORD_STATS(numGetSuccess++);
            //cout << "PithosTestApp: deleted key still available" << endl;
            return;
        } else {
            RECORD_STATS(numGetError++);
            //cout << "PithosTestApp: success (1)" << endl;
            return;
        }
    } else {
        delete context;

        EV << "Received result object: " << msg->getResult() << endl;

        if (msg->getResult() == *entry)
        {
            RECORD_STATS(numGetSuccess++);
            //cout << "PithosTestApp: success (2)" << endl;
            return;
        } else {
            RECORD_STATS(numGetError++);
            //cout << "PithosTestApp: wrong value" << endl;
            //cout << "value: " << msg->getResult(0).getValue() << endl;
            return;
        }
    }

}

void PithosTestApp::sendPutRequest()
{
	char name[41];

	GameObject *go = new GameObject("GameObject");
	go->setSize(exponential(objectSize_av));
	go->setCreationTime(simTime());

	//It is important that this name not contain spaces, otherwise a GameObject cannot be correctly reconstructed from a BinaryValue
	sprintf(name, "Node:%d,Object:%d", getParentModule()->getParentModule()->getIndex(), numPutSent);	//The name is later combined with the remaining object values
	go->setObjectName(name);
	go->setTTL(ttl);

	RootObjectPutCAPICall* capiPutMsg = new RootObjectPutCAPICall();

	//std::cout << "[PithosTestApp] Storing object with key " << go->getHash() << endl;

	capiPutMsg->addObject(go);
	capiPutMsg->setTtl(ttl);		//The TTL is set in the RPC call as well as the GameObject, for interoperability with the DHT application
	capiPutMsg->setIsModifiable(true);

	RECORD_STATS(numSent++; numPutSent++);
	sendInternalRpcCall(ROOTOBJECTSTORE_COMP, capiPutMsg, new PithosStatsContext(globalStatistics->isMeasuring(), simTime(), *go));
}

void PithosTestApp::handleLowerMessage (cMessage *msg)
{
	if (strcmp(msg->getName(), "request_start") == 0)
	{
		AddressPkt *request_start = check_and_cast<AddressPkt *>(msg);
		super_peer_address = request_start->getAddress();

		join_time = simTime() - wait_time;		//Adding join time gives every game module the same time to send successful requests

		if ((!pithostestput_timer->isScheduled()) && (!pithostestget_timer->isScheduled()) && (!pithostestmod_timer->isScheduled()))
		{
			if (simTime() < generationTime + wait_time + join_time)
			{
				sendPutRequest();

			    if (mean > 0) {
			        scheduleAt(simTime() + truncnormal(mean, deviation), pithostestput_timer);
			        scheduleAt(simTime() + truncnormal(mean + mean / 3, deviation), pithostestget_timer);
			        //scheduleAt(simTime() + truncnormal(mean + 2 * mean / 3, deviation), pithostestmod_timer);
			    }
			    else error("The mean message creation time must be greater than zero.");
			}
		} else error("Duplicate request start messages received");

		delete(msg);
	}
	else error("Game received unknown message\n");
}

OverlayKey PithosTestApp::getKey()
{
	if (intuniform(0,10) > 4)
	{
		OverlayKey key = globalPithosTestMap->getRandomGroupKey(super_peer_address);
		if (key.isUnspecified())
			return globalPithosTestMap->getRandomKey();
		return key;
	}
	else return globalPithosTestMap->getRandomKey();
}

void PithosTestApp::handleTimerEvent(cMessage* msg)
{
    if (msg->isName("pithostest_put_timer"))
    {
        // schedule next timer event
        scheduleAt(simTime() + truncnormal(mean, deviation), msg);

        // do nothing if the network is still in the initialization phase
        if (((!activeNetwInitPhase) && (underlayConfigurator->isInInitPhase()))
                || underlayConfigurator->isSimulationEndingSoon()
                || nodeIsLeavingSoon)
            return;

        if (simTime() > generationTime + wait_time + join_time)	//This has the module only generate requests for a finite amount of time
        	return;

        sendPutRequest();

    } else if (msg->isName("pithostest_get_timer"))
    {
        scheduleAt(simTime() + truncnormal(mean, deviation), msg);

        // do nothing if the network is still in the initialization phase
        if (((!activeNetwInitPhase) && (underlayConfigurator->isInInitPhase()))
                || underlayConfigurator->isSimulationEndingSoon()
                || nodeIsLeavingSoon) {
            return;
        }

        const OverlayKey& key = getKey();

        //std::cout << "[PithosTestApp] Retrieving object with key: " << key << endl;

        EV << "Retrieving key: " << key << endl;

        if (key.isUnspecified()) {
            EV << "[PithosTestApp::handleTimerEvent() @ " << thisNode.getIp()
               << " (" << thisNode.getKey().toString(16) << ")]\n"
               << "    Error: No key available in global DHT test map!"
               << endl;
            return;
        }

       	RootObjectGetCAPICall* capiGetMsg = new RootObjectGetCAPICall();
       	capiGetMsg->setKey(key);
        RECORD_STATS(numSent++; numGetSent++);

        sendInternalRpcCall(ROOTOBJECTSTORE_COMP, capiGetMsg, new PithosStatsContext(globalStatistics->isMeasuring(), simTime(), key));
    }/* else if (msg->isName("pithostest_mod_timer"))
    {
        scheduleAt(simTime() + truncnormal(mean, deviation), msg);

        // do nothing if the network is still in the initialization phase
        if (((!activeNetwInitPhase) && (underlayConfigurator->isInInitPhase()))
                || underlayConfigurator->isSimulationEndingSoon()
                || nodeIsLeavingSoon) {
            return;
        }

        const OverlayKey& key = globalDhtTestMap->getRandomKey();

        if (key.isUnspecified())
            return;

        DHTputCAPICall* dhtPutMsg = new DHTputCAPICall();
        dhtPutMsg->setKey(key);
        dhtPutMsg->setValue(generateRandomValue());
        dhtPutMsg->setTtl(ttl);
        dhtPutMsg->setIsModifiable(true);

        RECORD_STATS(numSent++; numPutSent++);
        sendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtPutMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), key, dhtPutMsg->getValue()));
    }*/
}

void PithosTestApp::handleNodeLeaveNotification()
{
    nodeIsLeavingSoon = true;
}

void PithosTestApp::finishApp()
{
    simtime_t time = globalStatistics->calcMeasuredLifetime(creationTime);

    if (time >= GlobalStatistics::MIN_MEASURED) {
    	// record scalar data
    	globalStatistics->addStdDev("PithosTestApp: Sent Total Messages", numSent);
		globalStatistics->addStdDev("PithosTestApp: Sent GET Messages", numGetSent);
		globalStatistics->addStdDev("PithosTestApp: Failed GET Requests", numGetError);
		globalStatistics->addStdDev("PithosTestApp: Successful GET Requests", numGetSuccess);
		globalStatistics->addStdDev("PithosTestApp: Sent PUT Messages", numPutSent);
		globalStatistics->addStdDev("PithosTestApp: Failed PUT Requests", numPutError);
		globalStatistics->addStdDev("PithosTestApp: Successful PUT Requests", numPutSuccess);

        if ((numGetSuccess + numGetError) > 0)
        {
            globalStatistics->addStdDev("PithosTestApp: GET Success Ratio", (double) numGetSuccess / (double) (numGetSuccess + numGetError));
        }
    }
}

