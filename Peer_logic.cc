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

	//Initialise queue statistics collection
	busySignal = registerSignal("busy");
	emit(busySignal, 0);

	event = new cMessage("event");
	scheduleAt(simTime()+par("wait_time"), event);

	latitude = uniform(0,100);		//Make this range changeable
	longitude = uniform(0,100);		//Make this range changeable
}

void Peer_logic::handleRequest(cMessage *msg)
{
	Message *m = check_and_cast<Message *>(msg);
	EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received store request of size " << m->getValue() << "\n";

	sendObjectForStore(m->getValue());
}

void Peer_logic::handleP2PMsg(cMessage *msg)
{
	char err_str[50];

	if ((strcmp(msg->getName(), "write") == 0) || (strcmp(msg->getName(), "replica_write") == 0))
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
	else if (strcmp(msg->getName(), "inform") == 0)
	{
		bootstrapPkt *boot_p = check_and_cast<bootstrapPkt *>(msg);

		super_peer_address = boot_p->getSuperPeerAdr();
		EV << "A new super peer has been identified at " << super_peer_address << endl;

		joinRequest(super_peer_address);
	}
	else if (strcmp(msg->getName(), "join_accept") == 0)
	{
		PeerListPkt *list_p = check_and_cast<PeerListPkt *>(msg);
		unsigned int i;

		for ( i = 0 ; i < list_p->getPeer_listArraySize() ; i++)
		{
			group_peers.push_back(list_p->getPeer_list(i));
		}

		EV << "Added " << list_p->getPeer_listArraySize() << " new peers to the list.\n";
	}
	else {
		sprintf(err_str, "Illegal P2P message received (%s)", msg->getName());
		error (err_str);
	}
}

void Peer_logic::joinRequest(TransportAddress dest_adr)
{
	bootstrapPkt *boot_p = new bootstrapPkt();
	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress *sourceAdr = new TransportAddress(thisNode.getIp(), thisNode.getPort());

	boot_p->setSourceAddress(*sourceAdr);
	boot_p->setDestinationAddress(dest_adr);
	boot_p->setPayloadType(JOIN_REQ);
	boot_p->setName("join_req");
	boot_p->setLatitude(latitude);
	boot_p->setLongitude(longitude);
	boot_p->setByteLength(4+4+4+8+8);	//Src IP as #, Dest IP as #, Type, Lat, Long

	send(boot_p, "comms_gate$o");

	//TODO: Add resend or timer that checks whether the join request has been handled by the Directory.
}

void Peer_logic::handleMessage(cMessage *msg)
{
	if (msg == event)
	{
		//For the first join request, a request is sent to the well known directory server
		IPAddress *dest_ip = new IPAddress(directory_ip);
		TransportAddress *destAdr = new TransportAddress(*dest_ip, directory_port);

		joinRequest(*destAdr);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "request") == 0)
	{
		//A storage request was received from the game or higher layer. This is data that should be stored in the network
		handleRequest(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
		handleP2PMsg(msg);
	}
	else error("Illegal message received");

	delete(msg);
}

void Peer_logic::GroupStore(groupPkt *write, GameObject *go)
{
	unsigned int i, j;
	simtime_t sendDelay;
	unsigned int group_replicas = par("replicas_g");
	GameObject *go_dup;
	groupPkt *write_dup;

	bool original_address;
	TransportAddress *send_list = new TransportAddress[group_replicas];

	TransportAddress dest_adr;

	//This ensures that an infinite while loop situation will never occur, but it also constrains the number of replicas to the number of known nodes
	if (group_replicas > group_peers.size())
			group_replicas = group_peers.size();

	for (i = 0 ; i < group_replicas ; i++)
	{
		//Duplicates the objects for sending
		go_dup = go->dup();
		write_dup = write->dup();

		original_address = false;

		while(!original_address)
		{
			dest_adr = ((PeerData)group_peers.at(intuniform(0, group_peers.size()-1))).getAddress();		//Choose a random peer in the group for the destination

			//Check all previous chosen addresses to determine whether this address is unique
			original_address = true;
			for (j = 0 ; j < i ; j++)
			{
				if (send_list[j] == dest_adr)
					original_address = false;
			}
		}

		if (i > 0) {
			go_dup->setType(REPLICA);
			write_dup->setName("replica_write");
		}
		else {
			write_dup->setName("write");
		}

		write_dup->addObject(go_dup);
		write_dup->setDestinationAddress(dest_adr);

		send_list[i] = dest_adr;

		send(write_dup, "comms_gate$o");		//Set sender address
		emit(busySignal, 0);
	}

	delete[]send_list;
}

void Peer_logic::OverlayStore(groupPkt *write, GameObject *go)
{
	simtime_t sendDelay;
	int overlay_replicas = par("replicas_sp");

	if (super_peer_address.isUnspecified())
	{
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		delete(write);
		delete(go);
		return;
	}

	go->setType(OVERLAY);

	write->setPayloadType(OVERLAY_WRITE_REQ);
	write->setValue(overlay_replicas);
	write->setName("overlay_write_req");
	write->setDestinationAddress(super_peer_address);
	write->addObject(go);

	send(write, "comms_gate$o");		//Set address
	emit(busySignal, 0);
}

void Peer_logic::sendObjectForStore(int64_t o_size)
{
	groupPkt *write;
	GameObject *go;

	//Create the packet that will house the game object
	write = new groupPkt();
	write->setByteLength(o_size);
	write->setPayloadType(WRITE);

	go = new GameObject("GameObject");
	go->setSize(o_size);

	//Send the message to be stored on the specified number of replicas in the group.
	GroupStore(write, go);

	OverlayStore(write, go);	//Send the message to be stored on the specified number of replicas in the overlay.
}
