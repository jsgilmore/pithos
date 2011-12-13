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

void PithosTestApp::sendPutRequest()
{
	char name[41];

	GameObject *go = new GameObject("GameObject");
	go->setSize(exponential(objectSize_av));
	go->setCreationTime(simTime());

	//It is important that this name not contain spaces, otherwise a GameObject cannot be correctly reconstructed from a BinaryValue
	sprintf(name, "Node:%d,Object:%d", getParentModule()->getParentModule()->getIndex(), numPutSent);	//The name is later combined with the remaining object values
	go->setObjectName(name);
	go->setType(ROOT);
	go->setTTL(ttl);

	RootObjectPutCAPICall* capiPutMsg = new RootObjectPutCAPICall();

	capiPutMsg->addObject(go);
	capiPutMsg->setTtl(ttl);		//The TTL is set in the RPC call as well as the GameObject, for interoperability with the DHT application
	capiPutMsg->setIsModifiable(true);

	RECORD_STATS(numSent++; numPutSent++);
	sendInternalRpcCall(ROOTOBJECTSTORE_COMP, capiPutMsg, new PithosStatsContext(globalStatistics->isMeasuring(), simTime(), *go));
	//sendInternalRpcCall(ROOTOBJECTSTORE_COMP, capiPutMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), destKey, dhtPutMsg->getValue()));
	//sendInternalRpcCall(ROOTOBJECTSTORE_COMP, capiPutMsg);
}

void PithosTestApp::handleLowerMessage (cMessage *msg)
{
	if (strcmp(msg->getName(), "request_start") == 0)
	{
		join_time = simTime() - wait_time;		//Adding join time gives every game module the same time to send successful requests

		if ((!pithostestput_timer->isScheduled()) && (!pithostestget_timer->isScheduled()) && (!pithostestmod_timer->isScheduled()))
		{
			if (simTime() < generationTime + wait_time + join_time)
			{
				sendPutRequest();

			    if (mean > 0) {
			        scheduleAt(simTime() + truncnormal(mean, deviation), pithostestput_timer);
			        //scheduleAt(simTime() + truncnormal(mean + mean / 3, deviation), pithostestget_timer);
			        //scheduleAt(simTime() + truncnormal(mean + 2 * mean / 3, deviation), pithostestmod_timer);
			    }
			    else error("The mean message creation time must be greater than zero.");
			}
		} else error("Duplicate request start messages received");

		delete(msg);
	}
	else error("Game received unknown message\n");
}

void PithosTestApp::handleRpcResponse(BaseResponseMessage* msg, const RpcState& state, simtime_t rtt)
{
    RPC_SWITCH_START(msg)
    RPC_ON_RESPONSE( RootObjectPutCAPI ) {
        handlePutResponse(_RootObjectPutCAPIResponse, check_and_cast<PithosStatsContext*>(state.getContext()));
        EV << "[PithosTestApp::handleRpcResponse()]\n"
           << "    DHT Put RPC Response received: id=" << state.getId()
           << " msg=" << *_RootObjectPutCAPIResponse << " rtt=" << rtt
           << endl;
        break;
    }
    /*RPC_ON_RESPONSE(DHTgetCAPI)
    {
        handleGetResponse(_DHTgetCAPIResponse, check_and_cast<DHTStatsContext*>(state.getContext()));
        EV << "[PithosTestApp::handleRpcResponse()]\n"
           << "    DHT Get RPC Response received: id=" << state.getId()
           << " msg=" << *_DHTgetCAPIResponse << " rtt=" << rtt
           << endl;
        break;
    }*/
    RPC_SWITCH_END()
}

void PithosTestApp::handlePutResponse(RootObjectPutCAPIResponse* msg, PithosStatsContext* context)
{
    GameObject object(context->go);

    globalPithosTestMap->insertEntry(object.getHash(), object);

    if (context->measurementPhase == false) {
        // don't count response, if the request was not sent
        // in the measurement phase
        delete context;
        return;
    }

    if (msg->getIsSuccess()) {
        RECORD_STATS(numPutSuccess++);
        RECORD_STATS(globalStatistics->addStdDev("PithosTestApp: PUT Latency (s)", SIMTIME_DBL(simTime() - context->requestTime)));
    } else {
        RECORD_STATS(numPutError++);
    }

    delete context;
}

/*
void PithosTestApp::handleGetResponse(DHTgetCAPIResponse* msg,
                                   DHTStatsContext* context)
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

    const DHTEntry* entry = globalDhtTestMap->findEntry(context->key);

    if (entry == NULL) {
        //unexpected key
        RECORD_STATS(numGetError++);
        //cout << "PithosTestApp: unexpected key" << endl;
        delete context;
        return;
    }

    if (simTime() > entry->endtime) {
        //this key doesn't exist anymore in the DHT, delete it in our hashtable

        globalDhtTestMap->eraseEntry(context->key);
        delete context;

        if (msg->getResultArraySize() > 0) {
            RECORD_STATS(numGetError++);
            //cout << "PithosTestApp: deleted key still available" << endl;
            return;
        } else {
            RECORD_STATS(numGetSuccess++);
            //cout << "PithosTestApp: success (1)" << endl;
            return;
        }
    } else {
        delete context;
        if ((msg->getResultArraySize() > 0) && (msg->getResult(0).getValue() != entry->value)) {
            RECORD_STATS(numGetError++);
            //cout << "PithosTestApp: wrong value" << endl;
            //cout << "value: " << msg->getResult(0).getValue() << endl;
            return;
        } else {
            RECORD_STATS(numGetSuccess++);
            //cout << "PithosTestApp: success (2)" << endl;
            return;
        }
    }

}*/

void PithosTestApp::handleTraceMessage(cMessage* msg)
{
    char* cmd = new char[strlen(msg->getName()) + 1];
    strcpy(cmd, msg->getName());

    if (strlen(msg->getName()) < 5) {
        delete[] cmd;
        delete msg;
        return;
    }

    if (strncmp(cmd, "PUT ", 4) == 0) {
        // Generate key
        char* buf = cmd + 4;

        while (!isspace(buf[0])) {
            if (buf[0] == '\0')
                throw cRuntimeError("Error parsing PUT command");
            buf++;
        }

        buf[0] = '\0';
        BinaryValue b(cmd + 4);
        OverlayKey destKey(OverlayKey::sha1(b));

        // get value
        buf++;

        RECORD_STATS(numSent++; numPutSent++);
        sendPutRequest();
    } /*else if (strncmp(cmd, "GET ", 4) == 0) {
        // Get key
        BinaryValue b(cmd + 4);
        OverlayKey key(OverlayKey::sha1(b));

        DHTgetCAPICall* dhtGetMsg = new DHTgetCAPICall();
        dhtGetMsg->setKey(key);
        RECORD_STATS(numSent++; numGetSent++);
        sendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtGetMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), key));
    }*/ else {
        throw cRuntimeError("Unknown trace command; only GET and PUT are allowed");
    }

    delete[] cmd;
    delete msg;
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


    } /*else if (msg->isName("pithostest_get_timer"))
    {
        scheduleAt(simTime() + truncnormal(mean, deviation), msg);

        // do nothing if the network is still in the initialization phase
        if (((!activeNetwInitPhase) && (underlayConfigurator->isInInitPhase()))
                || underlayConfigurator->isSimulationEndingSoon()
                || nodeIsLeavingSoon) {
            return;
        }

        const OverlayKey& key = globalDhtTestMap->getRandomKey();

        if (key.isUnspecified()) {
            EV << "[PithosTestApp::handleTimerEvent() @ " << thisNode.getIp()
               << " (" << thisNode.getKey().toString(16) << ")]\n"
               << "    Error: No key available in global DHT test map!"
               << endl;
            return;
        }

        DHTgetCAPICall* dhtGetMsg = new DHTgetCAPICall();
        dhtGetMsg->setKey(key);
        RECORD_STATS(numSent++; numGetSent++);

        sendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtGetMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), key));
    } else if (msg->isName("pithostest_mod_timer"))
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
/*
BinaryValue PithosTestApp::generateRandomValue()
{
    char value[DHTTESTAPP_VALUE_LEN + 1];

    for (int i = 0; i < DHTTESTAPP_VALUE_LEN; i++) {
        value[i] = intuniform(0, 25) + 'a';
    }

    value[DHTTESTAPP_VALUE_LEN] = '\0';
    return BinaryValue(value);
}*/

void PithosTestApp::handleNodeLeaveNotification()
{
    nodeIsLeavingSoon = true;
}

void PithosTestApp::finishApp()
{
    simtime_t time = globalStatistics->calcMeasuredLifetime(creationTime);

    if (time >= GlobalStatistics::MIN_MEASURED) {
        // record scalar data per second
        /*globalStatistics->addStdDev("PithosTestApp: Sent Total Messages/s", numSent / time);
        globalStatistics->addStdDev("PithosTestApp: Sent GET Messages/s", numGetSent / time);
        globalStatistics->addStdDev("PithosTestApp: Failed GET Requests/s", numGetError / time);
        globalStatistics->addStdDev("PithosTestApp: Successful GET Requests/s", numGetSuccess / time);
        globalStatistics->addStdDev("PithosTestApp: Sent PUT Messages/s", numPutSent / time);
        globalStatistics->addStdDev("PithosTestApp: Failed PUT Requests/s", numPutError / time);
        globalStatistics->addStdDev("PithosTestApp: Successful PUT Requests/s", numPutSuccess / time);*/

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

