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
	//Event does not have to be deleted here, since it is deleted in handleMessage
}

void Peer_logic::initialize()
{
	strcpy(directory_ip, par("directory_ip"));
	directory_port = par("directory_port");

	event = new cMessage("event");
	scheduleAt(simTime()+par("wait_time"), event);

	latitude = uniform(0,100);		//Make this range changeable
	longitude = uniform(0,100);		//Make this range changeable
}

void Peer_logic::finalize()
{

}

bool Peer_logic::hasSuperPeer()
{
	if (super_peer_address.isUnspecified())
		return false;
	else return true;
}

TransportAddress Peer_logic::getSuperPeerAddress()
{
	return super_peer_address;
}

void Peer_logic::handleP2PMsg(cMessage *msg)
{
	char err_str[50];

	Packet *packet = check_and_cast<Packet *>(msg);

	if (packet->getPayloadType() == INFORM)
	{
		bootstrapPkt *boot_p = check_and_cast<bootstrapPkt *>(msg);

		super_peer_address = boot_p->getSuperPeerAdr();
		EV << "A new super peer has been identified at " << super_peer_address << endl;

		cancelAndDelete(event);		//We've received the data from the directory server, so we can stop harassing them now

		joinRequest(super_peer_address);
	}
	else {
		sprintf(err_str, "Illegal P2P message received (%s)", msg->getName());
		error (err_str);
	}
}

void Peer_logic::handleGetCAPIRequest(RootObjectGetCAPICall* capiGetMsg)
{
	OverlayKeyPkt *read_pkt;
	Enter_Method("[Peer_logic]: handleGetCAPIRequest()");	//Required for Omnet++ context switching between modules
	take(capiGetMsg);

	EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received storage request for overlay key " << capiGetMsg->getKey() << "\n";

	read_pkt = new OverlayKeyPkt();
	read_pkt->setName(capiGetMsg->getName());
	read_pkt->setPayloadType(RETRIEVE_REQ);

	//This is the RPC ID of capiPutMsg and will be added to the response msg which the
	//peer logic can then use to match the received response to the relevant RPC call.
	read_pkt->setValue(capiGetMsg->getNonce());
	read_pkt->setKey(capiGetMsg->getKey());

	//Send the game object to be stored in the group.
	send(read_pkt->dup(), "group_write");

	//Send the game object to be stored in the overlay.
	send(read_pkt, "overlay_write");

    PendingRpcsEntry entry;
    entry.getCallMsg = capiGetMsg;
    entry.numSent = 1;		//TODO:This numSent should be calculated from the required group and overlay writes
    entry.state = LOOKUP_STARTED;
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
	entry.numSent = 4;		//TODO:This numSent should be calculated from the required group and overlay writes
	entry.putCallMsg = capiPutMsg;
	entry.state = PUT_SENT;
	pendingRpcs.insert(std::make_pair(capiPutMsg->getNonce(), entry));
}

void Peer_logic::joinRequest(const TransportAddress &dest_adr)
{
	if (dest_adr.isUnspecified())
		error("Destination address is unspecified when requesting a join.\n");

	bootstrapPkt *boot_p = new bootstrapPkt();
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	boot_p->setSourceAddress(sourceAdr);
	boot_p->setDestinationAddress(dest_adr);
	boot_p->setPayloadType(JOIN_REQ);
	boot_p->setName("join_req");
	boot_p->setLatitude(latitude);
	boot_p->setLongitude(longitude);
	boot_p->setByteLength(4+4+4+8+8);	//Src IP as #, Dest IP as #, Type, Lat, Long

	send(boot_p, "comms_gate$o");
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

	//Only determine success of failure after all responses have been received
	if (entry.numGroupGetSucceeded + entry.numGroupGetFailed +
			entry.numDHTGetSucceeded + entry.numDHTGetFailed == entry.numSent)
	{
		//Ensure that the number of successes of both group and overlay stores as greater than or equal to the number of failures
		if (entry.numDHTGetSucceeded >= entry.numDHTGetFailed)
		{
			if (entry.numGroupGetSucceeded >= entry.numGroupGetFailed)
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

				return;
			}
		}

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
			it->second.numGroupPutSucceeded++;
		else it->second.numGroupPutFailed++;

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
	if (msg == event)
	{
		//For the first join request, a request is sent to the well known directory server
		IPAddress dest_ip(directory_ip);
		TransportAddress destAdr(dest_ip, directory_port);

		joinRequest(destAdr);

		scheduleAt(simTime()+1, event);		//TODO: make the 1 second wait time a configuration variable that may be set
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "from_upperTier") == 0)
	{
		error("This gate has been closed down in favor of RPC calls");
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
		handleP2PMsg(msg);
		delete(msg);
	}
	else if ((strcmp(msg->getArrivalGate()->getName(), "overlay_read") == 0))
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
