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
	if (objectRepair && periodicRepair)
		cancelAndDelete(repairTimer);
}

void Super_peer_logic::initialize()
{
	numPeerArrivals = 0;
	numPeerDepartures = 0;

	globalStatistics = GlobalStatisticsAccess().get();

	strcpy(directory_ip, par("directory_ip"));
	directory_port = par("directory_port");

	//Initialise queue statistics collection
	//groupSizeSignal = registerSignal("GroupSize");
	//joinTimeSignal = registerSignal("JoinTime");

	cModule *groupLedgerModule = getParentModule()->getSubmodule("sp_group_ledger");
	group_ledger = check_and_cast<GroupLedger *>(groupLedgerModule);

    if (group_ledger == NULL) {
        throw cRuntimeError("Super peer logic::initializeApp(): Group ledger module not found!");
    }

	latitude = uniform(0,100);		//Make this range changeable
	longitude = uniform(0,100);		//Make this range changeable

	objectRepair = par("objectRepair");

	//A bool is used here for faster comparisons when the simulation is running.
	//Strings are used in the configuration to make the options more understandable.
	if (strcmp(par("repairType"), "periodic") == 0)
	{
		periodicRepair = true;
	}
	else if (strcmp(par("repairType"), "leaving") == 0)
	{
		periodicRepair = false;
	}else error("Invalid repair type specified. It should be \"leaving\", or \"periodic\"");

	if (objectRepair && periodicRepair)
	{
			repairTimer = new cMessage("repairTimer");
			repairTime = par("repairTime");
	}

	event = new cMessage("event");
	scheduleAt(simTime()+par("wait_time"), event);

	WATCH(numPeerArrivals);
	WATCH(numPeerDepartures);
}

void Super_peer_logic::finish()
{
	/*cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

    simtime_t time = globalStatistics->calcMeasuredLifetime(communicator->getCreationTime());

	std::ostringstream group_name;

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

	group_name << "Super_peer_logic (" << thisAdr << "): ";

    if (time >= GlobalStatistics::MIN_MEASURED) {
        // record scalar data
    	globalStatistics->addStdDev((group_name.str() + std::string("Number of peer arrivals")).c_str() , numPeerArrivals);
    	globalStatistics->addStdDev((group_name.str() + std::string("Number of peer departures")).c_str() , numPeerDepartures);
    }*/
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

	//emit(OverlayWriteSignal, 1);

	EV << "Packet sent for storage in the overlay\n";
}

void Super_peer_logic::addObject(PeerListPkt *plist_p)
{
	PeerData peer_data_recv;

	//Iterate through all PeerData objects received in the PeerListPkt
	for (unsigned int i = 0 ; i < plist_p->getPeer_listArraySize() ; i++)
	{
		peer_data_recv = ((PeerData)plist_p->getPeer_list(i));

		//Check whether this peer didn't just leave the group and this message is in fact a delayed out dated message
		if ((lastPeerLeft.getAddress().isUnspecified()) || (peer_data_recv != lastPeerLeft))
		{
			//Both the object and peer data have to be sent, because the two are linked in the ledger
			group_ledger->addObject(plist_p->getObjectData(), peer_data_recv);
		}
	}
}

void Super_peer_logic::informGroupPeers(bootstrapPkt *boot_req, TransportAddress sourceAdr)
{
	PeerData peer_data(boot_req->getSourceAddress());
	PeerListPkt *list_p = new PeerListPkt();

	list_p->setName("peer_join");
	list_p->setPayloadType(PEER_JOIN);
	list_p->setSourceAddress(sourceAdr);
	list_p->setGroupAddress(sourceAdr);
	list_p->setObjectData(ObjectData::UNSPECIFIED_OBJECT);
	list_p->addToPeerList(peer_data);
	list_p->setByteLength(PEERLIST_PKT_SIZE(PEERDATA_SIZE));

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
	list_p->setGroupAddress(sourceAdr);
	list_p->setDestinationAddress(boot_req->getSourceAddress());

	//Inform the joining peer of all peers in the group, in case there are peers that do not store any objects.
	//TODO: This should be changed to only inform the joining peer of the peers with no objects.
	list_p->setObjectData(ObjectData::UNSPECIFIED_OBJECT);
	list_p->setByteLength(PEERLIST_PKT_SIZE (PEERDATA_SIZE*(group_ledger->getGroupSize())));	//Peerlist packet size + peerdata inserted

	for (unsigned int i = 0 ; i < group_ledger->getGroupSize() ; i++)
	{
		list_p->addToPeerList(*(group_ledger->getPeerPtr(i)));	//Send a copy of the object, pointed to by the smart pointer
	}
	send(list_p->dup(), "comms_gate$o");	//Send a copy of the peer list, so the original packet may be reused to inform the other nodes
	list_p->clearPeerList();

	//If there are already objects stored in the group, inform the joining peer of all objects and of the peers they are stored on
	for (object_map_it = group_ledger->getObjectMapBegin() ; object_map_it != group_ledger->getObjectMapEnd() ; object_map_it++)
	{
		list_p->setObjectData(*(object_map_it->second.objectDataPtr));

		for (unsigned int j = 0 ; j < object_map_it->second.getPeerListSize() ; j++)
		{
			list_p->addToPeerList(*(object_map_it->second.getPeerRef(j)));	//Send a copy of the object, pointed to by the smart pointer
		}

		list_p->setByteLength(PEERLIST_PKT_SIZE (PEERDATA_SIZE*(object_map_it->second.getPeerListSize())));		//Peerlist packet size + peerdata inserted

		send(list_p->dup(), "comms_gate$o");	//Send a copy of the peer list, so the original packet may be reused to inform the other nodes
		list_p->clearPeerList();
	}

	delete(list_p);
}

void Super_peer_logic::handleJoinReq(cMessage *msg)
{
	bootstrapPkt *boot_req = check_and_cast<bootstrapPkt *>(msg);

	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress sourceAdr(thisNode.getIp(), thisNode.getPort());

	PeerData joining_peer(boot_req->getSourceAddress());

	//Multiple join requests can arrive, since peers retry upon timeout and sometimes the network is slow
	if (group_ledger->isPeerInGroup(joining_peer))
		return;

	//std::cout << "Super peer received bootstrap request from " << boot_req->getSourceAddress() << endl;

	//Inform all other nodes in the group of the joining node
	informGroupPeers(boot_req, sourceAdr);

	/**
	 * Add the data of the requesting peer into the list.
	 *
	 * Note the order of this function. Adding this IP here means the joining peer is informed of its own IP.
	 * This ensures that the joining peer only ads its own IP after successfully joining a group and only then
	 * informs the higher layer that it has successfully joined a group.
	**/
	group_ledger->addPeer(joining_peer);
	lastPeerJoined = joining_peer;
	RECORD_STATS(numPeerArrivals++);

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
	boot_p->setByteLength(BOOTSTRAP_PKT_SIZE);

	send(boot_p, "comms_gate$o");

	//emit(joinTimeSignal, simTime());

	//TODO: Add resend or timer that checks whether the join request has been handled by the Directory.
}

void Super_peer_logic::informLastJoinedOfLastLeft()
{
	PeerDataPkt *pkt = new PeerDataPkt("peerLeft");

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	pkt->setSourceAddress(sourceAdr);
	pkt->setGroupAddress(sourceAdr);
	pkt->setPayloadType(PEER_LEFT);
	pkt->setPeerData(lastPeerLeft);
	pkt->setByteLength(PEERDATA_PKT_SIZE);	//Source IP + Dest IP + type + left peer IP

	//Dereference it to get PeerDataPtr and use -> operator to get PeerData
	pkt->setDestinationAddress(lastPeerJoined.getAddress());

	send(pkt, "comms_gate$o");
}

void Super_peer_logic::repairMissingReplicas()
{
	ObjectLedgerMap::iterator object_map_it;
	ObjectData object_data;
	PeerData peer_data;
	int known_replicas;
	int expected_replicas = par("replicas");

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

	ReplicationReqPkt *replication_req = new ReplicationReqPkt();
	replication_req->setSourceAddress(thisAdr);
	replication_req->setPayloadType(REPLICATION_REQ);
	replication_req->setGroupAddress(thisAdr);
	replication_req->setByteLength(OBJECTDATA_PKT_SIZE);

	//Do this for every object in the object map
	for (object_map_it = group_ledger->getObjectMapBegin() ; object_map_it != group_ledger->getObjectMapEnd() ; object_map_it++)
	{
		object_data = *(object_map_it->second.objectDataPtr);
		replication_req->setObjectData(object_data);

		//Determine how many replications have to be made, by examining the current number of replicas.
		known_replicas = group_ledger->getReplicaNum(object_data);

		//There is no leaving peer, since these two occurrences are now independent, so equality has to be excluded
		if (known_replicas < expected_replicas)
		{
			replication_req->setReplicaDiff(expected_replicas-known_replicas);
			object_map_it->second.addRepairs(expected_replicas-known_replicas);		//Record the repairs performed for stat collection later.

			peer_data = group_ledger->getRandomPeer(object_data.getKey());

			replication_req->setDestinationAddress(peer_data.getAddress());
			send(replication_req->dup(), "comms_gate$o");
		}
	}

	delete(replication_req);
}

void Super_peer_logic::replicateObjectsOfPeer(PeerDataPkt *peer_data_pkt)
{
	ObjectData object_data;
	PeerData peer_data;
	int known_replicas;
	int expected_replicas = par("replicas");
	int objectLedgerSize = group_ledger->getObjectLedgerSize(peer_data_pkt->getPeerData());

	int peer_selection_tries;

	//If the peer did not contain any objects or the peer was not found (has already been removed) there is nothing to be done
	if (objectLedgerSize == 0 || objectLedgerSize == -1)
		return;

	ReplicationReqPkt *replication_req = new ReplicationReqPkt();
	replication_req->setSourceAddress(peer_data_pkt->getDestinationAddress());
	replication_req->setPayloadType(REPLICATION_REQ);
	replication_req->setGroupAddress(peer_data_pkt->getDestinationAddress());
	replication_req->setByteLength(OBJECTDATA_PKT_SIZE);

	//std::cout << "Super peer replicating objects on peer: " << peer_data_pkt->getPeerData().getAddress() << endl;

	//Every object housed on the leaving peer must be replicated
	for (int i = 0 ; i < objectLedgerSize ; i++)
	{
		//Find the ith object housed on the leaving peer
		object_data = group_ledger->getObjectFromPeer(peer_data_pkt->getPeerData(), i);
		replication_req->setObjectData(object_data);

		//Determine how many replications have to be made, by examining the current number of replicas.
		known_replicas = group_ledger->getReplicaNum(object_data);

		//The leaving peer hasn't been removed yet, so equality has to be included
		if (known_replicas <= expected_replicas)
		{
			replication_req->setReplicaDiff(expected_replicas-known_replicas+1);	//+1 for the leaving peer
			peer_selection_tries = 0;

			//Make sure the leaving peer isn't selected as the replicating peer
			while (1)
			{
				peer_data = group_ledger->getRandomPeer(object_data.getKey());
				if (peer_data != peer_data_pkt->getPeerData())
				{
					//const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
					//TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

					replication_req->setDestinationAddress(peer_data.getAddress());
					send(replication_req->dup(), "comms_gate$o");

					//std::cout << "[" << thisAdr << "]: Requesting replication of object (" << object_data.getObjectName() << ") with " << known_replicas << " known replicas on peer " << peer_data.getAddress() << endl;
					break;
				} else peer_selection_tries++;

				//This just prevents infinite loops when our replica numbers are too low
				if (peer_selection_tries > 2*expected_replicas)
					break;
			}
		}
	}

	delete(replication_req);
}

void Super_peer_logic::handlePeerLeaving(PeerData peer_data)
{
	group_ledger->removePeer(peer_data);
	RECORD_STATS(numPeerDepartures++);

	lastPeerLeft = peer_data;	//Record the data of the last peer that left, in case we get an outdated object add message from that peer

	//Inform the last peer that joined of the last peer that left, in case the peer that left did not know about the peer that joined.
	informLastJoinedOfLastLeft();

	//emit(groupSizeSignal, group_ledger->getGroupSize());
}

void Super_peer_logic::handleMessage(cMessage *msg)
{
	if (msg == event)
	{
		//Add the information of this super peer to the directory server
		addSuperPeer();

		if (objectRepair && periodicRepair)
		{
			//Initialise the repair timer
			scheduleAt(simTime(), repairTimer);
		}
		delete(msg);
	}
	else if (msg == repairTimer)
	{
		scheduleAt(simTime()+repairTime, repairTimer);

		repairMissingReplicas();
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		Packet *packet = check_and_cast<Packet *>(msg);

		if (packet->getPayloadType() == OVERLAY_WRITE_REQ)
		{
			handleOverlayWrite(msg);
		} else if (packet->getPayloadType() == SP_OBJECT_ADD)
		{
			PeerListPkt *plist_p = check_and_cast<PeerListPkt *>(msg);

			addObject(plist_p);
		} else if (packet->getPayloadType() == SP_PEER_LEFT)
		{
			PeerDataPkt *peer_data_pkt = check_and_cast<PeerDataPkt *>(packet);

			if (objectRepair && !periodicRepair)
			{
				replicateObjectsOfPeer(peer_data_pkt);
			}

			handlePeerLeaving(peer_data_pkt->getPeerData());

		} else if (packet->getPayloadType() == SP_PEER_MIGRATED)	//The super peer should not replicate objects if the migrating peer already has
		{
			PeerDataPkt *peer_data_pkt = check_and_cast<PeerDataPkt *>(packet);

			handlePeerLeaving(peer_data_pkt->getPeerData());

		} else if (packet->getPayloadType() == JOIN_REQ)
		{
			handleJoinReq(msg);

			//emit(groupSizeSignal, group_ledger->getGroupSize());
		} else error("Super peer received unknown group message from communicator");
		delete(msg);
	} else {
		char msg_str[100];
		sprintf(msg_str, "Unknown message received at Super peer logic (%s)", msg->getName());
		error(msg_str);
		delete(msg);
	}
}
