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

	groupSizeSignal = registerSignal("GroupSize");
	groupSendFailSignal = registerSignal("GroupSendFail");
	joinTimeSignal = registerSignal("JoinTime");

	//Initialise queue statistics collection
	busySignal = registerSignal("busy");
	emit(busySignal, 0);

	// initialize our statistics variables
	numSentForStore = 0;
	// tell the GUI to display our variables
	WATCH(numSentForStore);

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

void Peer_logic::addPeers(cMessage *msg)
{
	PeerListPkt *list_p = check_and_cast<PeerListPkt *>(msg);
	PeerData *peer_dat;
	unsigned int i;
	bool found;
	simtime_t joinTime = simTime();

	//If we didn't know of any peers in our group and we've now been informed of some, inform the game module to start producing requests
	//This is also the time when we record that we've successfully joined a group (When we've joined a super peer and we know of other peers in the group).
	if ((group_peers.size() == 0) && (list_p->getPeer_listArraySize() > 0))
	{
		cMessage *request_start = new cMessage("request_start");
		send(request_start, "game$o");

		emit(joinTimeSignal, joinTime);
	}

	for ( i = 0 ; i < list_p->getPeer_listArraySize() ; i++)
	{
		peer_dat = &(list_p->getPeer_list(i));
		found = false;

		//First check whether this peer is already known
		for (unsigned int j = 0 ; j < group_peers.size() ; j++)
		{
			if (group_peers.at(j) == *peer_dat)
			{
				found = true;
				break;
			}
		}

		//If the peer is not known, add it to the peer list
		if (!found)
		{
			group_peers.push_back(*peer_dat);
			emit(groupSizeSignal, 1);	//For every node sent back, our perceived group size increases by one.
		}
	}

	EV << "Added " << list_p->getPeer_listArraySize() << " new peers to the list.\n";
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

		cancelAndDelete(event);		//We've received the data from the directory server, so we can stop harassing them now

		joinRequest(super_peer_address);

		emit(groupSizeSignal, 1);	//The peer's perceived group size is one larger, because itself is part of the group it is in
	}
	else if (strcmp(msg->getName(), "join_accept") == 0)
	{
		addPeers(msg);
	}
	else {
		sprintf(err_str, "Illegal P2P message received (%s)", msg->getName());
		error (err_str);
	}
}

void Peer_logic::joinRequest(TransportAddress dest_adr)
{
	if (dest_adr.isUnspecified())
		error("Destination address is unspecified when requesting a join.\n");

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
}

void Peer_logic::handleMessage(cMessage *msg)
{
	if (msg == event)
	{
		//For the first join request, a request is sent to the well known directory server
		IPAddress *dest_ip = new IPAddress(directory_ip);
		TransportAddress *destAdr = new TransportAddress(*dest_ip, directory_port);

		joinRequest(*destAdr);

		scheduleAt(simTime()+1, event);		//TODO: make the 1 second wait time a configuration variable that may be set
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "game$i") == 0)
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

void Peer_logic::GroupStore(groupPkt *write, GameObject *go, std::vector<TransportAddress> send_list)
{
	unsigned int i, j;
	simtime_t sendDelay;
	GameObject *go_dup;
	groupPkt *write_dup;

	unsigned int group_replicas = par("replicas_g");

	bool original_address;

	TransportAddress dest_adr;

	//This ensures that an infinite while loop situation will never occur, but it also constrains the number of replicas to the number of known nodes
	if (group_replicas > group_peers.size())
	{
		emit(groupSendFailSignal, group_replicas - group_peers.size());
		group_replicas = group_peers.size();
	}

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
				if (send_list.at(j) == dest_adr)
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

		send_list.push_back(dest_adr);

		send(write_dup, "comms_gate$o");		//Set sender address
		emit(busySignal, 0);
	}

	delete(write);
}

void Peer_logic::OverlayStore(GameObject *go, std::vector<TransportAddress> send_list)
{
	simtime_t sendDelay;
	int overlay_replicas = par("replicas_sp");

	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress *sourceAdr = new TransportAddress(thisNode.getIp(), thisNode.getPort());

	PeerListPkt *overlay_write = new PeerListPkt();
	overlay_write->setByteLength(4+4+4+4+8+go->getSize()+(send_list.size()*4));	//Source address, dest address, type, value, object name ID, object size, storage peer addresses

	if (super_peer_address.isUnspecified())
	{
		//TODO: This error condition should be logged
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		delete(overlay_write);
		delete(go);
		return;
	}

	go->setType(OVERLAY);

	overlay_write->setPayloadType(OVERLAY_WRITE_REQ);
	overlay_write->setSourceAddress(*sourceAdr);
	overlay_write->setValue(overlay_replicas);
	overlay_write->setName("overlay_write_req");
	overlay_write->setDestinationAddress(super_peer_address);
	overlay_write->addObject(go);

	//Add the addresses of the other peers that have stored the data to the message
	for (unsigned int i = 0 ; i < send_list.size() ; i++)
	{
		PeerData peer_d;
		peer_d.setAddress(send_list.at(i));
		overlay_write->setPeer_list(i, peer_d);
	}

	send(overlay_write, "comms_gate$o");		//Set address
	emit(busySignal, 0);
}

//TODO: This branch of functions can be implemented much more elegantly
void Peer_logic::sendObjectForStore(int64_t o_size)
{
	groupPkt *write;
	GameObject *go;
	char name[41];

	std::vector<TransportAddress> send_list;

	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress *sourceAdr = new TransportAddress(thisNode.getIp(), thisNode.getPort());

	//Create the packet that will house the game object
	write = new groupPkt();
	write->setByteLength(4+4+4+8+o_size);	//Source address, dest address, type, object name ID and object size
	write->setPayloadType(WRITE);
	write->setSourceAddress(*sourceAdr);

	go = new GameObject("GameObject");
	go->setSize(o_size);
	go->setCreationTime(simTime());

	sprintf(name, "Game %d, Object %d, size %lld", getParentModule()->getIndex(), numSentForStore, o_size);
	go->setObjectName(name);

	EV << "Object to be sent: " << go->getObjectName() << endl;

	//Send the message to be stored on the specified number of replicas in the group.
	GroupStore(write, go, send_list);

	OverlayStore(go, send_list);	//Send the message to be stored on the specified number of replicas in the overlay.

	numSentForStore++;
}
