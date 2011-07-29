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

#include "Peer_logic.h"

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

	// initialize our statistics variables
	numSentForStore = 0;
	// tell the GUI to display our variables
	WATCH(numSentForStore);

	event = new cMessage("event");
	scheduleAt(simTime()+par("wait_time"), event);

	latitude = uniform(0,100);		//Make this range changeable
	longitude = uniform(0,100);		//Make this range changeable
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

	if (packet->getPayloadType() == WRITE)
	{

		groupPkt *group_p = check_and_cast<groupPkt *>(msg);

		cMessage *storage_msg = new cMessage("storage");
		GameObject *go = (GameObject *)group_p->removeObject("GameObject");

		if (go->getType() == ROOT)
			EV << getName() << " " << getIndex() << " received root Game Object of size " << go->getSize() << "\n";
		else if (go->getType() == REPLICA)
			EV << getName() << " " << getIndex() << " received replica Game Object of size " << go->getSize() << "\n";

		storage_msg->addObject(go);
		send(storage_msg, "write");
	}
	else if (packet->getPayloadType() == INFORM)
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

void Peer_logic::handleRequest(cMessage *msg)
{
	groupPkt *m = check_and_cast<groupPkt *>(msg);

	GameObject *go;
	char name[41];

	EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received store request of size " << m->getValue() << "\n";

	go = new GameObject("GameObject");
	go->setSize(m->getValue());
	go->setCreationTime(simTime());

	sprintf(name, "Game %d, Object %d, size %ld", getParentModule()->getIndex(), numSentForStore, m->getValue());	//TODO: The name should actually be all object contents
	go->setObjectName(name);

	EV << "Object to be sent: " << go->getObjectName() << endl;

	msg->addObject(go);

	//Send the game object to be stored in the group.
	send(msg->dup(), "group_store");

	//Send the game object to be stored in the overlay.
	send(msg, "overlay_store");

	numSentForStore++;
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
		//A storage request was received from the game or higher layer. This is data that should be stored in the network
		handleRequest(msg);
		delete(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
		handleP2PMsg(msg);
		delete(msg);
	}
	else {
		error("Illegal message received");
		delete(msg);
	}
}
