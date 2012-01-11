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

#include "Super_peer_logic.h"

Super_peer_logic::Super_peer_logic()
{
}

Super_peer_logic::~Super_peer_logic()
{
}

void Super_peer_logic::initialize()
{
	strcpy(directory_ip, par("directory_ip"));
	directory_port = par("directory_port");

	//Initialise queue statistics collection
	OverlayWriteSignal = registerSignal("OverlayWrite");
	groupSizeSignal = registerSignal("GroupSize");
	OverlayDeliveredSignal =  registerSignal("OverlayDelivered");
	joinTimeSignal = registerSignal("JoinTime");
	storeNumberSignal  = registerSignal("StoreNumber");
	overlayNumberSignal = registerSignal("OverlayNumber");

	overlaysStoreFailSignal =  registerSignal("overlaysStoreFail");

	latitude = uniform(0,100);		//Make this range changeable
	longitude = uniform(0,100);		//Make this range changeable

	event = new cMessage("event");
	scheduleAt(simTime()+par("wait_time"), event);
}

void Super_peer_logic::finish()
{

}

void Super_peer_logic::handleOverlayWrite(cMessage *msg)
{
	overlayPkt *overlay_p;
	ValuePkt *overlay_write_req = check_and_cast<ValuePkt *>(msg);
	GameObject *go = (GameObject *)overlay_write_req->removeObject("GameObject");


	if (go == NULL)
		error("No game object found attached to the message\n");

	//Send the game objects into the overlay
	//FIXME: The hash string should still be adapted to allow for multiple replicas in the overlay
	for (unsigned int i = 0; i < overlay_write_req->getValue(); i++)
	{
		overlay_p = new overlayPkt(); // the message we'll send
		overlay_p->setType(OVERLAY_WRITE); // set the message type to OVERLAY_WRITE
		overlay_p->setByteLength((go->getSize()+4) + 4 + 20);	//Game object size and type + packet type + routing key
		overlay_p->setName("overlay_write");

		overlay_p->addObject(go->dup());

		send(overlay_p, "comms_gate$o"); // send it to the overlay
	}
	delete(go);

	emit(OverlayWriteSignal, 1);

	EV << "Packet sent for storage in the overlay\n";
}

void Super_peer_logic::addObject(cMessage *msg)
{
	PeerListPkt *plist_p = check_and_cast<PeerListPkt *>(msg);
	std::vector<PeerDataPtr>::iterator it;
	PeerData peer_data_recv;

	//Log the file name and what peers it is stored on
	ObjectInfo object_info;
	object_info.setObjectName(plist_p->getObjectName());
	object_info.setSize(plist_p->getObjectSize());

	for (unsigned int i = 0 ; i < plist_p->getPeer_listArraySize() ; i++)
	{
		peer_data_recv = ((PeerData)plist_p->getPeer_list(i));

		for (it = group_peers.begin() ; it != group_peers.end() ; it++)
		{
			//*it returns a PeerDataPtr type, which again has to be dereferenced to obtain the PeerData object (**it)
			if (**it == peer_data_recv)
				break;
		}

		object_info.addPeerRef(*it);
	}

	object_map.insert(std::make_pair(plist_p->getObjectKey(), object_info));

	emit(storeNumberSignal, 1);
}

void Super_peer_logic::handleJoinReq(cMessage *msg)
{
	unsigned int i;
	bootstrapPkt *boot_req = check_and_cast<bootstrapPkt *>(msg);
	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress sourceAdr(thisNode.getIp(), thisNode.getPort());

	EV << "Super peer received bootstrap request from " << boot_req->getSourceAddress() << ", sending list and updating group.\n";

	//IP data entry for the requesting peer to be added to the group peers list.
	PeerDataPtr peer_dat_ptr(new PeerData());

	//List packet that will be returned to the requesting peer
	PeerListPkt *list_p = new PeerListPkt();

	//Set the values of the packet to be returned to the requesting peer.
	//Set the type and byte length
	list_p->setName("join_accept");
	list_p->setPayloadType(JOIN_ACCEPT);
	list_p->setByteLength(2*sizeof(int)+sizeof(int)*2*group_peers.size());	//Value+Type+(IP+Port)*list_length FIXME: The size needs to still be multiplied by the size of the peer list.
	list_p->setSourceAddress(sourceAdr);
	list_p->setDestinationAddress(boot_req->getSourceAddress());

	for (i = 0 ; i < group_peers.size() ; i++)
	{
		list_p->addToPeerList(*(group_peers.at(i)));	//Send a copy of the object, pointed to by the smart pointer
	}
	send(list_p->dup(), "comms_gate$o");	//Send a copy of the peer list, so the original packet may be reused to inform the other nodes

	(*peer_dat_ptr).setAddress(boot_req->getSourceAddress());
	list_p->clearPeerList();	//This erases all data added to the peer list.
	list_p->addToPeerList(*peer_dat_ptr);
	list_p->setByteLength(2*sizeof(int)+sizeof(int)*2);	//Value+Type+(IP+Port)

	for (i = 0 ; i < group_peers.size() ; i++)
	{
		//group_peers.at(i) returns a PeerDataPtr, which has to be dereferenced to return a PeerData object
		list_p->setDestinationAddress((*(group_peers.at(i))).getAddress());

		send(list_p->dup(), "comms_gate$o");
	}
	delete(list_p);

	//Add the data of the requesting peer into the list.
	group_peers.push_back(peer_dat_ptr);

	//The original message is deleted in the calling function.
}

void Super_peer_logic::GroupStore(overlayPkt *overlay_p)
{
	TransportAddress dest_adr;
	GameObject *go = (GameObject *)overlay_p->removeObject("GameObject");
	ValuePkt *group_p;
	ObjectInfo object_info;
	PeerDataPtr peer_dat_ptr;

	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress sourceAdr(thisNode.getIp(), thisNode.getPort());

	if (group_peers.size() > 0)
	{
		int peer_nr =  intuniform(0, group_peers.size()-1);		//Choose a random peer in the group for the destination
		peer_dat_ptr = group_peers.at(peer_nr);
		dest_adr = (*peer_dat_ptr).getAddress();
	}
	else {
		emit(overlaysStoreFailSignal, 1);
		delete(go);
		return;
	}

	//Log the file name and what peers it is stored on
	object_info.setObjectName(go->getObjectName());
	object_info.setSize(go->getSize());
	object_info.addPeerRef(peer_dat_ptr);
	object_map.insert(std::make_pair(go->getHash(), object_info));
	emit(overlayNumberSignal, 1);

	go->setType(OVERLAY);

	group_p = new ValuePkt();
	group_p->setSourceAddress(sourceAdr);
	group_p->setDestinationAddress(dest_adr);
	group_p->setPayloadType(WRITE);
	group_p->setName("write");
	group_p->setByteLength(4+4+4 + go->getSize());	//Src IP as #, Dest IP as #, Type, Game Object size
	group_p->addObject(go);

	send(group_p, "comms_gate$o");		//Set sender address

	//The original message is deleted in the calling function.
}

void Super_peer_logic::addSuperPeer()
{
	IPAddress dest_ip(directory_ip);
	TransportAddress dest_adr(dest_ip, directory_port);

	if (dest_adr.isUnspecified())
		error("Destination address is unspecified when trying to add this Super peer.\n");

	bootstrapPkt *boot_p = new bootstrapPkt();
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	boot_p->setSourceAddress(sourceAdr);
	boot_p->setDestinationAddress(dest_adr);
	boot_p->setPayloadType(SUPER_PEER_ADD);
	boot_p->setName("super_peer_add");
	boot_p->setLatitude(latitude);
	boot_p->setLongitude(longitude);
	boot_p->setByteLength(4+4+4+8+8);	//Src IP as #, Dest IP as #, Type, Lat, Long

	send(boot_p, "comms_gate$o");

	emit(joinTimeSignal, simTime());

	//TODO: Add resend or timer that checks whether the join request has been handled by the Directory.
}

void Super_peer_logic::handleMessage(cMessage *msg)
{
	if (msg == event)
	{
		//Add the information of this super peer to the directory server
		addSuperPeer();
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		Packet *packet = check_and_cast<Packet *>(msg);

		if (packet->getPayloadType() == OVERLAY_WRITE_REQ)
		{
			handleOverlayWrite(msg);
		}
		else if (packet->getPayloadType() == OBJECT_ADD)
		{
			addObject(msg);
		}
		else if (packet->getPayloadType() == JOIN_REQ)
		{
			handleJoinReq(msg);

			emit(groupSizeSignal, group_peers.size());
		}
		else error("Super peer received unknown group message from communicator");
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "overlay_gate$i") == 0)
	{
		overlayPkt *overlay_p = check_and_cast<overlayPkt *>(msg);

		GroupStore(overlay_p);

		emit(OverlayDeliveredSignal, 1);
	}
	else {
		char msg_str[100];
		sprintf(msg_str, "Unknown message received at Super peer logic (%s)", msg->getName());
		error(msg_str);
	}

	delete(msg);
}
