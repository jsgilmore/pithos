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
	replicas = par("replicas");
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
	read_pkt->setByteLength(4 + 4 + 4 + 4 + 4);		//payload type + source address + destination address + rpcid + key

	//These duplicate addresses of the current node are used by group storage to determine whether the higher layer has requested an object, or another peer
	read_pkt->setSourceAddress(address);
	read_pkt->setDestinationAddress(address);

	//This is the RPC ID of capiPutMsg and will be added to the response msg which the
	//peer logic can then use to match the received response to the relevant RPC call.
	read_pkt->setValue(capiGetMsg->getNonce());
	read_pkt->setKey(capiGetMsg->getKey());

	//std::cout << "[Peer_logic] Retrieving object with key: " << capiGetMsg->getKey() << endl;

	read_pkt->setHops(0);

	//Send the game object to be stored in the group.
	send(read_pkt->dup(), "group_write");

	//Send the game object to be stored in the overlay.
	send(read_pkt, "overlay_write");

    PendingRpcsEntry entry;
    entry.getCallMsg = capiGetMsg;
    entry.numSent = 1;		//Only one of the responses from either DHT storage or group storage is required
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

	//Send the game object to be stored in the group.
	send(write_pkt->dup(), "group_write");

	//Send the game object to be stored in the overlay.
	send(write_pkt, "overlay_write");

	//Add the received RPC to the list of RPC for which responses are still outstanding
	PendingRpcsEntry entry;
	entry.numSent = replicas + 1;		//Number of replicas required for group storage plus the ons DHT storage request
	entry.putCallMsg = capiPutMsg;
	pendingRpcs.insert(std::make_pair(capiPutMsg->getNonce(), entry));
}

void Peer_logic::processPut(PendingRpcsEntry entry, ResponsePkt *response)
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	//This extra step ensures that the submodules exist and also does any other required error checking
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	//Ensure that the number of successes of both group and overlay stores as greater than or equal to the number of failures
	if (entry.numGroupPutSucceeded + entry.numGroupPutFailed +
			entry.numDHTPutSucceeded + entry.numDHTPutFailed == entry.numSent)
	{
		if (entry.numDHTPutSucceeded > entry.numDHTPutFailed)
		{
			//Ensure that more group puts succeeded than failed
			if (entry.numGroupPutSucceeded >= entry.numGroupPutFailed)
			{
				RootObjectPutCAPIResponse* capiPutRespMsg = new RootObjectPutCAPIResponse();
				capiPutRespMsg->setIsSuccess(true);
				capiPutRespMsg->setGroupAddress(entry.group_address);
				communicator->externallySendRpcResponse(entry.putCallMsg, capiPutRespMsg);
				pendingRpcs.erase(response->getRpcid());

				return;
			}
		}

		//This is the failure response to both situations where either the group messages
		//failed or the overlay messages failed. Notice the "return" in the success scenario.
		RootObjectPutCAPIResponse* capiPutRespMsg = new RootObjectPutCAPIResponse();
		capiPutRespMsg->setIsSuccess(false);
		communicator->externallySendRpcResponse(entry.putCallMsg, capiPutRespMsg);
		pendingRpcs.erase(response->getRpcid());
	}
}

void Peer_logic::processGet(PendingRpcsEntry entry, ResponsePkt *response)
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	//This extra step ensures that the submodules exist and also does any other required error checking
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

	if ((entry.numGroupGetSucceeded == 1) || (entry.numDHTGetSucceeded == 1))
	{
		//TODO: Attach the actual GameObject to the RPC response, instead of copying it into a new object
		GameObject *object = (GameObject *)response->getObject("GameObject");
		if (object == NULL)
			error("No object was attached to DHT Storage response message");

		RootObjectGetCAPIResponse* capiGetRespMsg = new RootObjectGetCAPIResponse();
		capiGetRespMsg->setIsSuccess(true);
		capiGetRespMsg->setResult(*object);	//The value is copied here and not the actual object
		communicator->externallySendRpcResponse(entry.getCallMsg, capiGetRespMsg);
		pendingRpcs.erase(response->getRpcid());

	} else if ((entry.numDHTGetFailed == 1) && (entry.numGroupGetFailed == 1))
	{
		//This is the failure response to both situations where either the group messages
		//failed or the overlay messages failed. Notice the "return" in the success scenario.
		RootObjectGetCAPIResponse* capiGetRespMsg = new RootObjectGetCAPIResponse();
		capiGetRespMsg->setIsSuccess(false);
		communicator->externallySendRpcResponse(entry.getCallMsg, capiGetRespMsg);
		pendingRpcs.erase(response->getRpcid());
	}
}

void Peer_logic::handleResponseMsg(cMessage *msg)
{
	ResponsePkt *response = check_and_cast<ResponsePkt *>(msg);

	PendingRpcs::iterator it = pendingRpcs.find(response->getRpcid());

	if (it == pendingRpcs.end()) // unknown request or request for already erased call
		return;

	//Check what type of response was received and increment the successes or failures according to type
	//Then process the RPC relating to the response, by checking whether sufficient responses were received, taking action if this was the case
	if (response->getResponseType() == GROUP_PUT)
	{
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

		processGet(it->second, response);
	} else if (response->getResponseType() == GROUP_GET)
	{
		if (response->getIsSuccess())
			it->second.numGroupGetSucceeded++;
		else it->second.numGroupGetFailed++;

		processGet(it->second, response);
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
