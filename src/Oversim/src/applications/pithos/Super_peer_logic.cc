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

	cModule *groupLedgerModule = getParentModule()->getSubmodule("sp_group_ledger");
	group_ledger = check_and_cast<GroupLedger *>(groupLedgerModule);

    if (group_ledger == NULL) {
        throw cRuntimeError("Super peer logic::initializeApp(): Group ledger module not found!");
    }

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
	PeerData peer_data_recv;

	//Iterate through all PeerData objects received in the PeerListPkt
	for (unsigned int i = 0 ; i < plist_p->getPeer_listArraySize() ; i++)
	{
		peer_data_recv = ((PeerData)plist_p->getPeer_list(i));

		//Both the object and peer data have to be sent, because the two are linked in the ledger
		group_ledger->addObject(plist_p->getObjectData(), peer_data_recv);
	}
}

void Super_peer_logic::informGroupPeers(bootstrapPkt *boot_req, TransportAddress sourceAdr)
{
	PeerData peer_data(boot_req->getSourceAddress());
	PeerListPkt *list_p = new PeerListPkt();

	list_p->setName("peer_join");
	list_p->setPayloadType(PEER_JOIN);
	list_p->setSourceAddress(sourceAdr);
	list_p->setObjectData(ObjectData::UNSPECIFIED_OBJECT);
	list_p->addToPeerList(peer_data);
	list_p->setByteLength(4 + 4 + 4);	//Value+Type+IP

	for (unsigned int i = 0 ; i < group_ledger->getGroupSize() ; i++)
	{
		//group_peers.at(i) returns a PeerDataPtr, which has to be dereferenced to return a PeerData object
		list_p->setDestinationAddress((*(group_ledger->getPeerPtr(i))).getAddress());

		send(list_p->dup(), "comms_gate$o");
	}

	delete(list_p);
}

void Super_peer_logic::informJoiningPeer(bootstrapPkt *boot_req, TransportAddress sourceAdr)
{
	PeerListPkt *list_p = new PeerListPkt();
	ObjectLedgerMap::iterator object_map_it;

	list_p->setName("join_accept");
	list_p->setPayloadType(JOIN_ACCEPT);
	list_p->setSourceAddress(sourceAdr);
	list_p->setDestinationAddress(boot_req->getSourceAddress());

	//If there are already objects stored in the group, inform the joining peer of all objects and of the peers they are stored on
	for (object_map_it = group_ledger->getObjectMapBegin() ; object_map_it != group_ledger->getObjectMapEnd() ; object_map_it++)
	{
		list_p->setObjectData(*(object_map_it->second.objectDataPtr));

		for (unsigned int j = 0 ; j < object_map_it->second.getPeerListSize() ; j++)
		{
			list_p->addToPeerList(*(object_map_it->second.getPeerRef(j)));	//Send a copy of the object, pointed to by the smart pointer
		}

		list_p->setByteLength(2*sizeof(int)+sizeof(ObjectData)+sizeof(int)*(object_map_it->second.getPeerListSize()));	//Value+Type+ObjectData+(IP)*list_length

		send(list_p->dup(), "comms_gate$o");	//Send a copy of the peer list, so the original packet may be reused to inform the other nodes
		list_p->clearPeerList();
	}

	list_p->setObjectData(ObjectData::UNSPECIFIED_OBJECT);
	list_p->setByteLength(2*sizeof(int)+sizeof(ObjectData)+sizeof(int)*(group_ledger->getGroupSize()));	//Value+Type+ObjectData+(IP)*list_length

	//If there are no objects stored in the group, only inform the joining peer of the other peers if any
	for (unsigned int i = 0 ; i < group_ledger->getGroupSize() ; i++)
	{
		list_p->addToPeerList(*(group_ledger->getPeerPtr(i)));	//Send a copy of the object, pointed to by the smart pointer
	}
	send(list_p, "comms_gate$o");	//Send a copy of the peer list, so the original packet may be reused to inform the other nodes
}

void Super_peer_logic::handleJoinReq(cMessage *msg)
{
	bootstrapPkt *boot_req = check_and_cast<bootstrapPkt *>(msg);
	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress sourceAdr(thisNode.getIp(), thisNode.getPort());

	EV << "Super peer received bootstrap request from " << boot_req->getSourceAddress() << ", sending list and updating group.\n";

	//Inform all other nodes in the group of the joining node
	informGroupPeers(boot_req, sourceAdr);

	/**
	 * Add the data of the requesting peer into the list.
	 *
	 * Note the order of this function. Adding this IP here means the joining peer is informed of its own IP.
	 * This ensures that the joining peer only ads its own IP after successfully joining a group and only then
	 * informs the higher layer that it has successfully joined a group.
	**/
	group_ledger->addPeer(PeerData(boot_req->getSourceAddress()));

	informJoiningPeer(boot_req, sourceAdr);

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
		} else if (packet->getPayloadType() == SP_OBJECT_ADD)
		{
			addObject(msg);
		} else if (packet->getPayloadType() == SP_PEER_LEFT)
		{
			PeerDataPkt *peer_data_pkt = check_and_cast<PeerDataPkt *>(packet);
			group_ledger->removePeer(peer_data_pkt->getPeerData());

			emit(groupSizeSignal, group_ledger->getGroupSize());
		} else if (packet->getPayloadType() == JOIN_REQ)
		{
			handleJoinReq(msg);

			emit(groupSizeSignal, group_ledger->getGroupSize());
		} else error("Super peer received unknown group message from communicator");
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "overlay_gate$i") == 0)
	{
		error("Unknown message received from overlay.");
	}
	else {
		char msg_str[100];
		sprintf(msg_str, "Unknown message received at Super peer logic (%s)", msg->getName());
		error(msg_str);
	}

	delete(msg);
}
