//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "DHTStorage.h"

#include <GlobalNodeListAccess.h>
#include <GlobalStatisticsAccess.h>
#include <UnderlayConfiguratorAccess.h>
#include <RpcMacros.h>

#include <GlobalDhtTestMap.h>

DHTStorage::DHTStorage() {
	// TODO Auto-generated constructor stub

}

DHTStorage::~DHTStorage() {
	// TODO Auto-generated destructor stub
}

void DHTStorage::initialize()
{
	// fetch parameters
	activeNetwInitPhase = par("activeNetwInitPhase");

	ttl = par("testTtl");

	globalNodeList = GlobalNodeListAccess().get();
	underlayConfigurator = UnderlayConfiguratorAccess().get();
	globalStatistics = GlobalStatisticsAccess().get();

	globalDhtTestMap = dynamic_cast<GlobalDhtTestMap*>(simulation.getModuleByPath("globalObserver.globalFunctions[0].function"));

	if (globalDhtTestMap == NULL) {
		throw cRuntimeError("DHTStorage::initializeApp(): GlobalDhtTestMap module not found!");
	}

	// statistics
	numSent = 0;
	numPutSent = 0;
	numPutError = 0;
	numPutSuccess = 0;
	numGetSent = 0;
	numGetError = 0;
	numGetSuccess = 0;

	//initRpcs();
	WATCH(numSent);
	WATCH(numPutSent);
	WATCH(numPutError);
	WATCH(numPutSuccess);
	WATCH(numGetSent);
	WATCH(numGetError);
	WATCH(numGetSuccess);

	nodeIsLeavingSoon = false;
}

void DHTStorage::handleTraceMessage(cMessage* msg)
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	//This extra step ensures that the submodules exist and also does any other required error checking
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	Enter_Method("[DHTStorage]: handleTraceMessage()");	//Required for Omnet++ context switching between modules
	take(msg);	//This module should first take ownership of the received message before that message can be resent

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

        // build putMsg
        DHTputCAPICall* dhtPutMsg = new DHTputCAPICall();
        dhtPutMsg->setKey(destKey);
        dhtPutMsg->setValue(buf);
        dhtPutMsg->setTtl(ttl);
        dhtPutMsg->setIsModifiable(true);
        RECORD_STATS(numSent++; numPutSent++);
        communicator->externallySendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtPutMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), destKey));
    } else if (strncmp(cmd, "GET ", 4) == 0) {
        // Get key
        BinaryValue b(cmd + 4);
        OverlayKey key(OverlayKey::sha1(b));

        DHTgetCAPICall* dhtGetMsg = new DHTgetCAPICall();
        dhtGetMsg->setKey(key);
        RECORD_STATS(numSent++; numGetSent++);
        communicator->externallySendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtGetMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), key));
    } else {
        throw cRuntimeError("Unknown trace command; "
                                "only GET and PUT are allowed");
    }

    delete[] cmd;
    delete msg;
}

void DHTStorage::handleRpcResponse(BaseResponseMessage* msg, const RpcState& state, simtime_t rtt)
{
	Enter_Method("[DHTStorage]: handleRpcResponse()");	//Required for Omnet++ context switching between modules
	take(msg);	//This module should first take ownership of the received message before that message can be resent

	RPC_SWITCH_START(msg)
	   RPC_ON_RESPONSE( DHTputCAPI ) {
		   handlePutResponse(_DHTputCAPIResponse, check_and_cast<DHTStatsContext*>(state.getContext()));

		   EV << "[DHTStorage::handleRpcResponse()]\n"
			  << "    DHT Put RPC Response received: id=" << state.getId()
			  << " msg=" << *_DHTputCAPIResponse << " rtt=" << rtt
			  << endl;
		   break;
	   }
	   /*RPC_ON_RESPONSE(DHTgetCAPI)
	   {
		   handleGetResponse(_DHTgetCAPIResponse,
							 check_and_cast<DHTStatsContext*>(state.getContext()));
		   EV << "[DHTStorage::handleRpcResponse()]\n"
			  << "    DHT Get RPC Response received: id=" << state.getId()
			  << " msg=" << *_DHTgetCAPIResponse << " rtt=" << rtt
			  << endl;
		   break;
	   }*/
	   RPC_SWITCH_END()
}

void DHTStorage::handlePutResponse(DHTputCAPIResponse* msg, DHTStatsContext* context)
{
    DHTEntry entry = {context->value, simTime() + ttl};

    //TODO: The complete global DHT test map should be removed from the DHT storage component for the real-world application.
    //This is ONLY required to test the correctness of the overlay itself and is effectively storing all data inserted into the overlay two-fold.
    globalDhtTestMap->insertEntry(context->key, entry);

    if (context->measurementPhase == false) {
        // don't count response, if the request was not sent
        // in the measurement phase
        delete context;
        return;
    }

    if (msg->getIsSuccess()) {
        RECORD_STATS(numPutSuccess++);
        RECORD_STATS(globalStatistics->addStdDev("DHTTestApp: PUT Latency (s)", SIMTIME_DBL(simTime() - context->requestTime)));
    } else {
        RECORD_STATS(numPutError++);
    }

    delete context;
}

void DHTStorage::store(GameObject *go)
{
	CSHA1 hash;
	char hash_str[41];		//SHA-1 produces a 160 bit/20 byte hash

	cModule *this_peerModule = getParentModule()->getSubmodule("peer_logic");
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");

	//This extra step ensures that the submodules exist and also does any other required error checking
	Peer_logic *this_peer = check_and_cast<Peer_logic *>(this_peerModule);
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	if (!(this_peer->hasSuperPeer()))
	{
		//TODO: This error condition should be logged
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		delete(go);
		return;
	}

	for (int i = 0 ; i < 41 ; i++)	//The string has to be cleared for the OverlayKey constructor to correctly handle it.
		hash_str[i] = 0;

	EV << "Sending object with name: " << go->getObjectName() << endl;

	//Create a hash of the game object's name
	hash.Update(((unsigned char *)go->info().c_str()), strlen(go->info().c_str()));
	hash.Final();
	hash.ReportHash(hash_str, CSHA1::REPORT_HEX);

	go->setType(OVERLAY);

	//OverlayKey destKey = OverlayKey::random();	//For testing purposes only
	OverlayKey destKey(hash_str, 16);

	DHTputCAPICall* dhtPutMsg = new DHTputCAPICall();
	dhtPutMsg->setKey(destKey);
	dhtPutMsg->setValue(go->getBinaryValue());
	dhtPutMsg->setTtl(ttl);
	dhtPutMsg->setIsModifiable(true);
	//dhtPutMsg->setByteLength(4+4+4+4+8+go->getSize()); 	//Source address, dest address, type, value, object name ID, object size

	RECORD_STATS(numSent++; numPutSent++);
	communicator->externallySendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtPutMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), destKey, dhtPutMsg->getValue()));

	delete(go);		//The GameObject was not sent, rather its string representation as a BinaryValue (required for the Oversim DHT)
}

void DHTStorage::handleMessage(cMessage *msg)
{
	GameObject *go = (GameObject *)msg->removeObject("GameObject");

	if (go == NULL)
		error("No object was attached to be stored in group storage");

	store(go);

	delete(msg);
}

void DHTStorage::finish()
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

    simtime_t time = globalStatistics->calcMeasuredLifetime(communicator->getCreationTime());

    if (time >= GlobalStatistics::MIN_MEASURED) {
        // record scalar data
        globalStatistics->addStdDev("DHTStorage: Sent Total Messages/s",
                                    numSent / time);

        globalStatistics->addStdDev("DHTStorage: Sent PUT Messages/s",
                                    numPutSent / time);
        globalStatistics->addStdDev("DHTStorage: Failed PUT Requests/s",
                                    numPutError / time);
        globalStatistics->addStdDev("DHTStorage: Successful PUT Requests/s",
                                    numPutSuccess / time);

        if ((numGetSuccess + numGetError) > 0) {
            globalStatistics->addStdDev("DHTStorage: GET Success Ratio",
                                        (double) numGetSuccess
                                        / (double) (numGetSuccess + numGetError));
        }
    }
}
