//
// Copyright (C) 2011 MIH Media lab, University of Stellenbosch
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

#include "Peer_logic.h"

Define_Module(Peer_logic);

Peer_logic::Peer_logic()
{
}

Peer_logic::~Peer_logic()
{
    PendingRpcs::iterator it;

    for (it = pendingRpcs.begin(); it != pendingRpcs.end(); it++) {
        delete(it->second.putCallMsg);
        delete(it->second.getCallMsg);
    }

    pendingRpcs.clear();
}

void Peer_logic::initialize()
{
	globalStatistics = GlobalStatisticsAccess().get();

	replicas = par("replicas");
	numGetRequests = par("numGetRequests");
	numGetCompares = par("numGetCompares");
	disableDHT = par("disableDHT");
	std::string putType = par("putType");
	std::string getType = par("getType");

	if (putType == "fast")
		fastPut = true;
	else if (putType == "safe")
		fastPut = false;
	else error("Unknown put type specified.");

	if (getType == "fast")
		fastGet = true;
	else if (getType == "safe")
		fastGet = false;
	else error("Unknown get type specified.");
}

void Peer_logic::finalize()
{

}

void Peer_logic::handleGetCAPIRequest(RootObjectGetCAPICall* capiGetMsg)
{
	OverlayKeyPkt *read_pkt;
	Enter_Method("[Peer_logic]: handleGetCAPIRequest()");	//Required for Omnet++ context switching between modules
	take(capiGetMsg);

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress address(thisNode->getIp(), thisNode->getPort());

	EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received storage request for overlay key " << capiGetMsg->getKey() << "\n";

	read_pkt = new OverlayKeyPkt();
	read_pkt->setName(capiGetMsg->getName());
	read_pkt->setPayloadType(RETRIEVE_REQ);
	read_pkt->setByteLength(OVERLAYKEY_PKT_SIZE);

	//These duplicate addresses of the current node are used by group storage to determine whether the higher layer has requested an object, or another peer
	read_pkt->setSourceAddress(address);
	read_pkt->setDestinationAddress(address);

	//This is the RPC ID of capiPutMsg and will be added to the response msg which the
	//peer logic can then use to match the received response to the relevant RPC call.
	read_pkt->setValue(capiGetMsg->getNonce());
	read_pkt->setKey(capiGetMsg->getKey());

	read_pkt->setTimestamp(capiGetMsg->getCreationTime()); //Record the creation time of the original request.

	//std::cout << "[Peer_logic] Retrieving object with key: " << capiGetMsg->getKey() << endl;

	read_pkt->setHops(0);

	//Send the game object to be stored in the group.
	send(read_pkt->dup(), "group_write");

	//Send the game object to be stored in the overlay.
	if (!disableDHT)
		send(read_pkt, "overlay_write");
	else delete(read_pkt);

    PendingRpcsEntry entry;
    entry.getCallMsg = capiGetMsg;
    entry.numSent = numGetRequests;
    pendingRpcs.insert(std::make_pair(capiGetMsg->getNonce(), entry));
}

void Peer_logic::handlePutCAPIRequest(RootObjectPutCAPICall* capiPutMsg)
{
	ValuePkt *write_pkt;
	Enter_Method("[Peer_logic]: handlePutCAPIRequest()");	//Required for Omnet++ context switching between modules
	take(capiPutMsg);

	GameObject *go = (GameObject *)capiPutMsg->removeObject("GameObject");
	if (go == NULL)
		error("No object was attached to be stored in group storage");

	EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received game object to store of size " << go->getSize() << "\n";

	EV << "Object to be sent: " << go->getObjectName() << endl;

	//std::cout << "[Peer_logic] Storing object with key " << go->getHash() << endl;

	write_pkt = new ValuePkt();
	write_pkt->setName(capiPutMsg->getName());
	write_pkt->setPayloadType(STORE_REQ);
	write_pkt->addObject(go);

	//This is the RPC ID of capiPutMsg and will be added to the response msg which the
	//peer logic can then use to match the received response to the relevant RPC call.
	write_pkt->setValue(capiPutMsg->getNonce());

	write_pkt->setTimestamp(capiPutMsg->getCreationTime()); //Record the creation time of the original request.

	//Send the game object to be stored in the group.
	send(write_pkt->dup(), "group_write");

	PendingRpcsEntry entry;
	entry.putCallMsg = capiPutMsg;

	//Send the game object to be stored in the overlay, if this option was not disabled
	if (!disableDHT)
	{
		send(write_pkt, "overlay_write");
		entry.numSent = replicas + 1;		//Number of replicas required for group storage plus the ons DHT storage request
	} else {
		delete(write_pkt);
		entry.numSent = replicas;			//Number of replicas required for group storage
	}

	//Add the received RPC to the list of RPC for which responses are still outstanding
	pendingRpcs.insert(std::make_pair(capiPutMsg->getNonce(), entry));
}

void Peer_logic::processPut(PendingRpcsEntry entry, ResponsePkt *response)
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	//This extra step ensures that the submodules exist and also does any other required error checking
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	//Remember that these variables have been set false here, when reading the rest of the code
	bool complete = false;
	bool success = false;

	//If Safe put was selected
	if (!fastPut)
	{
		//Ensure that the number of successes of both group and overlay stores as greater than or equal to the number of failures
		if (entry.numGroupPutSucceeded + entry.numGroupPutFailed +
				entry.numDHTPutSucceeded + entry.numDHTPutFailed == entry.numSent)
		{
			complete = true;
			//Ensure that more group puts succeeded than failed
			if (entry.numGroupPutSucceeded >= entry.numGroupPutFailed)
			{
				if ((entry.numDHTPutSucceeded > entry.numDHTPutFailed) || disableDHT)
				{
					success = true;
				}
			}
		}
	//If Fast put was selected
	} else {
		//For fast puts we just want one successful group or DHT put for a success
		if (entry.numGroupPutSucceeded == 1 || entry.numDHTPutSucceeded == 1)
		{
			complete = true;
			success = true;
		}
		else if (entry.numGroupPutFailed + entry.numDHTPutFailed == entry.numSent)
		{
			complete = true;
		}
	}

	if (complete)
	{
		RootObjectPutCAPIResponse* capiPutRespMsg = new RootObjectPutCAPIResponse();
		capiPutRespMsg->setGroupAddress(entry.group_address);

		if (success) capiPutRespMsg->setIsSuccess(true);
		else capiPutRespMsg->setIsSuccess(false);

		communicator->externallySendRpcResponse(entry.putCallMsg, capiPutRespMsg);
		pendingRpcs.erase(response->getRpcid());
	}
}

GameObject Peer_logic::pickObject(std::vector<GameObject>objectsReceived)
{
	//A vector of GameObjects and the number peers that returned that object
	std::map<GameObject, int> objectValueMap;
	std::vector<GameObject>::iterator received_object_it;
	std::map<GameObject, int>::iterator counting_object_it;
	std::map<GameObject, int>::iterator picked_object_it;


	for (received_object_it = objectsReceived.begin() ; received_object_it != objectsReceived.end() ; received_object_it++)
	{
		counting_object_it = objectValueMap.find(*received_object_it);
		if (counting_object_it == objectValueMap.end())
		{
			objectValueMap.insert(std::make_pair(*received_object_it, 1));
		} else {
			counting_object_it->second = counting_object_it->second +1;
		}
	}

	picked_object_it = objectValueMap.begin();
	for (counting_object_it = objectValueMap.begin() ; counting_object_it != objectValueMap.end() ; counting_object_it++)
	{
		if (counting_object_it->second > picked_object_it->second)
		{
			picked_object_it = counting_object_it;
		}
	}

	//There must at least have been some majority
	if (picked_object_it->second > 1)
		return picked_object_it->first;
	else return GameObject::UNSPECIFIED_OBJECT;
}

void Peer_logic::processGet(PendingRpcsEntry *entry, ResponsePkt *response)
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	//This extra step ensures that the submodules exist and also does any other required error checking
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	if (fastGet)
	{
		if ((entry->numGroupGetSucceeded == 1) || (entry->numDHTGetSucceeded == 1))
		{
			//TODO: Attach the actual GameObject to the RPC response, instead of copying it into a new object
			GameObject *object = (GameObject *)response->getObject("GameObject");
			if (object == NULL)
				error("No object was attached to DHT Storage response message");

			RootObjectGetCAPIResponse* capiGetRespMsg = new RootObjectGetCAPIResponse();
			capiGetRespMsg->setIsSuccess(true);
			capiGetRespMsg->setResult(*object);	//The value is copied here and not the actual object
			communicator->externallySendRpcResponse(entry->getCallMsg, capiGetRespMsg);
			pendingRpcs.erase(response->getRpcid());
		//If both the DHT get and all group gets failed, or DHT is disabled and all group gets failed, a failure occurred
		} else if (((entry->numDHTGetFailed == 1) || disableDHT) && (entry->numGroupGetFailed == numGetRequests))
		{
			//This is the failure response to both situations where either the group messages
			//failed or the overlay messages failed. Notice the "return" in the success scenario.
			RootObjectGetCAPIResponse* capiGetRespMsg = new RootObjectGetCAPIResponse();
			capiGetRespMsg->setIsSuccess(false);
			communicator->externallySendRpcResponse(entry->getCallMsg, capiGetRespMsg);
			pendingRpcs.erase(response->getRpcid());
		}
	} else {

		if (response->getIsSuccess())
		{
			GameObject *object = (GameObject *)response->getObject("GameObject");
			if (object == NULL)
				error("No object was attached to DHT Storage response message");

			//Add the received object to the received objects vector for comparison
			entry->objectsReceived.push_back(*object);
		}

		if (entry->numGroupGetSucceeded + entry->numDHTGetSucceeded == numGetCompares)
		{
			GameObject object = pickObject(entry->objectsReceived);

			RootObjectGetCAPIResponse* capiGetRespMsg = new RootObjectGetCAPIResponse();

			if (object != GameObject::UNSPECIFIED_OBJECT)
			{
				capiGetRespMsg->setIsSuccess(true);
				capiGetRespMsg->setResult(object);	//The value is copied here and not the actual object
			}
			//If it could not be determined which was the correct object, don't send any object
			else capiGetRespMsg->setIsSuccess(false);

			communicator->externallySendRpcResponse(entry->getCallMsg, capiGetRespMsg);

			pendingRpcs.erase(response->getRpcid());
		//If both the DHT get and the group get failed, or DHT is disabled and group get failed, a failure occurred
		} else if (((entry->numDHTGetFailed == 1) || disableDHT) && (entry->numGroupGetFailed == numGetRequests))
		{
			//This is the failure response to both situations where either the group messages
			//failed or the overlay messages failed. Notice the "return" in the success scenario.
			RootObjectGetCAPIResponse* capiGetRespMsg = new RootObjectGetCAPIResponse();
			capiGetRespMsg->setIsSuccess(false);
			communicator->externallySendRpcResponse(entry->getCallMsg, capiGetRespMsg);
			pendingRpcs.erase(response->getRpcid());
		}
	}
}

void Peer_logic::handleResponseMsg(cMessage *msg)
{
	ResponsePkt *response = check_and_cast<ResponsePkt *>(msg);

	PendingRpcs::iterator it = pendingRpcs.find(response->getRpcid());

	//Pithos has already decided whether the originating request succeeded or failed, so already informed the higher layer and removed this pending entry.
	if (it == pendingRpcs.end())
		return;

	//Check what type of response was received and increment the successes or failures according to type
	//Then process the RPC relating to the response, by checking whether sufficient responses were received, taking action if this was the case
	if (response->getResponseType() == GROUP_PUT)
	{
		if (response->getTimestamp() == 0.0)
			error("PUT response received recorded a zero time stamp.");
		RECORD_STATS(globalStatistics->recordOutVector("GroupStorage: PUT Latency (s)", SIMTIME_DBL(simTime() - response->getTimestamp())));

		if (response->getIsSuccess())
		{
			it->second.numGroupPutSucceeded++;

			//Check whether a group address is known.
			if (it->second.group_address.isUnspecified())
			{
				//If not, record it.
				//This logs the group address to enable the higher layer to generate group specific requests
				it->second.group_address = response->getGroupAddress();
			}
			else if (it->second.group_address != response->getGroupAddress())
			{
				//If it is, check that both match.
				error("Group address received does not match known group address");
			}
			//Group addresses are not checked for a failure, since the failure could have been caused by a packet sent to the wrong group.
		} else it->second.numGroupPutFailed++;

		processPut(it->second, response);
	} else if (response->getResponseType() == OVERLAY_PUT)
	{
		if (response->getIsSuccess())
			it->second.numDHTPutSucceeded++;
		else it->second.numDHTPutFailed++;

		processPut(it->second, response);
	} else if (response->getResponseType() == OVERLAY_GET)
	{
		if (response->getIsSuccess())
			it->second.numDHTGetSucceeded++;
		else it->second.numDHTGetFailed++;

		processGet(&(it->second), response);
	} else if (response->getResponseType() == GROUP_GET)
	{
		if (response->getTimestamp() == 0.0)
			error("GET response received recorded a zero time stamp.");
		RECORD_STATS(globalStatistics->recordOutVector("GroupStorage: GET Latency (s)", SIMTIME_DBL(simTime() - response->getTimestamp())));

		if (response->getIsSuccess())
			it->second.numGroupGetSucceeded++;
		else it->second.numGroupGetFailed++;

		processGet(&(it->second), response);
	} else error("Unknown response type received");
}

void Peer_logic::handleMessage(cMessage *msg)
{

	if ((strcmp(msg->getArrivalGate()->getName(), "overlay_read") == 0))
	{
		//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
		handleResponseMsg(msg);
		delete(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "group_read") == 0)
	{
		//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
		handleResponseMsg(msg);
		delete(msg);
	}
	else {
		error("Illegal message received");
		delete(msg);
	}
}
