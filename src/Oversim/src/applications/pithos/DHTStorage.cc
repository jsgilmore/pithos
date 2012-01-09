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

// register module class with `\opp`
Define_Module(DHTStorage);

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

	globalDhtTestMap = dynamic_cast<GlobalDhtTestMap*>(simulation.getModuleByPath("globalObserver.globalFunctions[1].function"));

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

void DHTStorage::handleGetResponse(DHTgetCAPIResponse* msg, DHTStatsContext* context)
{
	if (context->measurementPhase == false) {
		// don't count response, if the request was not sent
		// in the measurement phase
		delete context;
		return;
	}

	RECORD_STATS(globalStatistics->addStdDev("DHTStorage: GET Latency (s)",
							   SIMTIME_DBL(simTime() - context->requestTime)));

	if (!(msg->getIsSuccess())) {
		//cout << "DHTTestApp: success == false" << endl;
		RECORD_STATS(numGetError++);
		sendResponse(OVERLAY_GET, context->parent_rpcid, false);
		delete context;
		return;
	}

	EV << "Successfully retrieved key: " << context->key << endl;

	const DHTEntry* entry = globalDhtTestMap->findEntry(context->key);


	//This can occur when a TTL expired
	if (entry == NULL) {
		//unexpected key
		RECORD_STATS(numGetError++);
		sendResponse(OVERLAY_GET, context->parent_rpcid, false);
		//cout << "DHTTestApp: unexpected key" << endl;
		delete context;
		return;
	}

	if (simTime() > entry->endtime) {
		//this key doesn't exist anymore in the DHT, delete it in our hashtable

		globalDhtTestMap->eraseEntry(context->key);
		delete context;

		if (msg->getResultArraySize() > 0) {
			RECORD_STATS(numGetError++);
			sendResponse(OVERLAY_GET, context->parent_rpcid, false);
			//cout << "DHTTestApp: deleted key still available" << endl;
			return;
		} else {
			RECORD_STATS(numGetSuccess++);
			sendResponse(OVERLAY_GET, context->parent_rpcid, true);	//A success is returned here, although no data are attached, since the data has expired
			//cout << "DHTTestApp: success (1)" << endl;
			return;
		}
	} else {
		delete context;
		if (msg->getResultArraySize() > 0)
		{
			if (msg->getResult(0).getValue() != entry->value)
			{
				RECORD_STATS(numGetError++);
				sendResponse(OVERLAY_GET, context->parent_rpcid, false);
				//cout << "DHTTestApp: wrong value" << endl;
				//cout << "value: " << msg->getResult(0).getValue() << endl;
				return;
			} else {
				RECORD_STATS(numGetSuccess++);
				sendResponse(OVERLAY_GET, context->parent_rpcid, true, msg->getResult(0).getValue());
				//cout << "DHTTestApp: success (2)" << endl;
				return;
			}
		} else {
			//For some reason, there are occasions when the underlying DHT reports a successful retrieval, without attaching any result
			RECORD_STATS(numGetError++);
			sendResponse(OVERLAY_GET, context->parent_rpcid, false);
		}
	}
}

void DHTStorage::sendResponse(int responseType, unsigned int rpcid, bool isSuccess, const BinaryValue& value)
{
	GameObject *object = new GameObject(value);

	EV << "[DHTStorage] returning result: " << object << endl;

	ResponsePkt *response = new ResponsePkt();
	response->setResponseType(responseType);
	response->setPayloadType(RESPONSE);
	response->setRpcid(rpcid);		//This allows the higher layer to know which RPC call is being acknowledged.
	response->setIsSuccess(isSuccess);
	response->addObject(object);

	send(response, "read");
}

void DHTStorage::handlePutResponse(DHTputCAPIResponse* msg, DHTStatsContext* context)
{
    DHTEntry entry = {context->object.getBinaryValue(), simTime() + context->object.getTTL()};

    //TODO: The complete global DHT test map should be removed from the DHT storage component for the real-world application.
    //This is ONLY required to test the correctness of the overlay itself and is effectively storing all data inserted into the overlay two-fold.
    globalDhtTestMap->insertEntry(context->key, entry);

    EV << "Storing DHT entry: " << context->object.getBinaryValue() << endl;

    if (context->measurementPhase == false) {
        // don't count response, if the request was not sent
        // in the measurement phase
        delete context;
        return;
    }

    if (msg->getIsSuccess()) {
        RECORD_STATS(numPutSuccess++);
        RECORD_STATS(globalStatistics->addStdDev("DHTTestApp: PUT Latency (s)", SIMTIME_DBL(simTime() - context->requestTime)));
        sendResponse(OVERLAY_PUT, context->parent_rpcid, true);
    } else {
        RECORD_STATS(numPutError++);
        sendResponse(OVERLAY_PUT, context->parent_rpcid, false);
    }

    delete context;
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
	   RPC_ON_RESPONSE(DHTgetCAPI)
	   {
		   handleGetResponse(_DHTgetCAPIResponse, check_and_cast<DHTStatsContext*>(state.getContext()));

		   EV << "[DHTStorage::handleRpcResponse()]\n"
			  << "    DHT Get RPC Response received: id=" << state.getId()
			  << " msg=" << *_DHTgetCAPIResponse << " rtt=" << rtt
			  << endl;
		   break;
	   }
	   RPC_SWITCH_END()
}

void DHTStorage::request_retrieve(Packet *pkt)
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	//This extra step ensures that the submodules exist and also does any other required error checking
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	OverlayKeyPkt *read_pkt = check_and_cast<OverlayKeyPkt *>(pkt);

	OverlayKey destkey = read_pkt->getKey();

	unsigned int parent_rpcid = read_pkt->getValue();	//The RPC ID of the original request received from above the Pithos layer

	DHTgetCAPICall* dhtGetMsg = new DHTgetCAPICall();
    dhtGetMsg->setKey(destkey);
    RECORD_STATS(numSent++; numGetSent++);

	communicator->externallySendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtGetMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), destkey, parent_rpcid));
}

void DHTStorage::send_forstore(Packet *pkt)
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	//This extra step ensures that the submodules exist and also does any other required error checking
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	ValuePkt *write_pkt = check_and_cast<ValuePkt *>(pkt);

	GameObject *go = (GameObject *)pkt->removeObject("GameObject");
	if (go == NULL)
		error("No object was attached to be stored in group storage");

	OverlayKey destkey = go->getHash();		//Setting the hash here, means that it's the same hash as the root object type stored by the higher layer.

	unsigned int parent_rpcid = write_pkt->getValue();	//The RPC ID of the original request received from above the Pithos layer

	//go->setType(OVERLAY);		//Having an explicit overlay type screws with the hashes requested by the higher layer

	//TODO: Update the lower layer so that it retrieves the required information directly from the GameObject
	DHTputCAPICall* dhtPutMsg = new DHTputCAPICall();
	dhtPutMsg->setKey(destkey);
	dhtPutMsg->setValue(go->getBinaryValue());
	dhtPutMsg->setTtl(go->getTTL());
	dhtPutMsg->setIsModifiable(true);
	dhtPutMsg->setByteLength(4+4+4+4+8+go->getSize()); 	//Source address, dest address, type, value, object name ID, object size

	RECORD_STATS(numSent++; numPutSent++);

	EV << "Sending object with name: " << go->getObjectName() << endl;
	communicator->externallySendInternalRpcCall(OVERLAYSTORAGE_COMP, dhtPutMsg, new DHTStatsContext(globalStatistics->isMeasuring(), simTime(), destkey, parent_rpcid, *go));

	delete(go);
}

void DHTStorage::handleMessage(cMessage *msg)
{
	Packet *pkt = check_and_cast<Packet *>(msg);

	if (pkt->getPayloadType() == STORE_REQ)
	{
		send_forstore(pkt);
	} else if (pkt->getPayloadType() == RETRIEVE_REQ)
	{
		request_retrieve(pkt);
	}

	delete(msg);
}

void DHTStorage::finish()
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

    simtime_t time = globalStatistics->calcMeasuredLifetime(communicator->getCreationTime());

    if (time >= GlobalStatistics::MIN_MEASURED) {
        // record scalar data
        globalStatistics->addStdDev("DHTStorage: Sent Total Messages/s", numSent / time);

        globalStatistics->addStdDev("DHTStorage: Sent PUT Messages/s", numPutSent / time);
        globalStatistics->addStdDev("DHTStorage: Failed PUT Requests/s", numPutError / time);
        globalStatistics->addStdDev("DHTStorage: Successful PUT Requests/s", numPutSuccess / time);

        globalStatistics->addStdDev("DHTStorage: Sent GET Messages/s", numGetSent / time);
		globalStatistics->addStdDev("DHTStorage: Failed GET Requests/s", numGetError / time);
		globalStatistics->addStdDev("DHTStorage: Successful GET Requests/s", numGetSuccess / time);

        if ((numGetSuccess + numGetError) > 0) {
            globalStatistics->addStdDev("DHTStorage: GET Success Ratio", (double) numGetSuccess / (double) (numGetSuccess + numGetError));
        }
    }

}
