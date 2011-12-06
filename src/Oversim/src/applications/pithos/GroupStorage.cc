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

#include "GroupStorage.h"
#include <GlobalStatisticsAccess.h>

Define_Module(GroupStorage);

GroupStorage::GroupStorage() {
	// TODO Auto-generated constructor stub

}

GroupStorage::~GroupStorage() {
	// TODO Auto-generated destructor stub
}

void GroupStorage::initialize()
{
	groupSizeSignal = registerSignal("GroupSize");
	groupSendFailSignal = registerSignal("GroupSendFail");
	joinTimeSignal = registerSignal("JoinTime");

	/*globalStatistics = GlobalStatisticsAccess().get();

	// statistics
	numSent = 0;
	numPutSent = 0;
	numPutError = 0;
	numPutSuccess = 0;

	//initRpcs();
	WATCH(numSent);
	WATCH(numPutSent);
	WATCH(numPutError);
	WATCH(numPutSuccess);*/
}

void GroupStorage::updateSuperPeerObjects(const char *objectName, unsigned long objectSize, std::vector<TransportAddress> send_list)
{
	simtime_t sendDelay;
	Peer_logic * this_peer = ((Peer_logic *)getParentModule()->getSubmodule("peer_logic"));

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	PeerListPkt *objectAddPkt = new PeerListPkt();
	objectAddPkt->setByteLength(4+4+4+8+(send_list.size()*4));	//Source address, dest address, type, object name ID, storage peer addresses


	if (!(this_peer->hasSuperPeer()))
	{
		//TODO: This error condition should be logged
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		delete(objectAddPkt);
		return;
	}

	objectAddPkt->setObjectName(objectName);
	objectAddPkt->setObjectSize(objectSize);
	objectAddPkt->setPayloadType(OBJECT_ADD);
	objectAddPkt->setSourceAddress(sourceAdr);
	objectAddPkt->setName("object_add");
	objectAddPkt->setDestinationAddress(this_peer->getSuperPeerAddress());

	//Add the addresses of the other peers that have stored the data to the message
	for (unsigned int i = 0 ; i < send_list.size() ; i++)
	{
		PeerData peer_d;
		peer_d.setAddress(send_list.at(i));
		objectAddPkt->addToPeerList(peer_d);
	}

	send(objectAddPkt, "comms_gate$o");		//Set address
}

int GroupStorage::getReplicaNr()
{
	unsigned int replicas = par("replicas");

	//This ensures that an infinite while loop situation will never occur, but it also constrains the number of replicas to the number of known nodes
	if (replicas > group_peers.size())
	{
		emit(groupSendFailSignal, replicas - group_peers.size());
		//RECORD_STATS(numPutError++);
		replicas = group_peers.size();
	}

	return replicas;
}

void GroupStorage::createWritePkt(groupPkt **write)
{
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	//Create the packet that will house the game object
	(*write) = new groupPkt();
	(*write)->setByteLength(4+4+4+8);	//Source address, dest address, type, object name ID and object size
	(*write)->setPayloadType(WRITE);
	(*write)->setSourceAddress(sourceAdr);
}

void GroupStorage::selectDestination(TransportAddress *dest_adr, std::vector<TransportAddress> send_list)
{
	unsigned int j;
	bool original_address = false;

	while(!original_address)
	{
		*dest_adr = ((PeerData)group_peers.at(intuniform(0, group_peers.size()-1))).getAddress();		//Choose a random peer in the group for the destination

		//Check all previous chosen addresses to determine whether this address is unique
		original_address = true;

		for (j = 0 ; j < send_list.size() ; j++)
		{
			if (send_list.at(j) == *dest_adr)
				original_address = false;
		}
	}
}

void GroupStorage::store(GameObject *go)
{
	unsigned int i;
	simtime_t sendDelay;
	GameObject *go_dup;
	groupPkt *write = NULL;
	groupPkt *write_dup;

	unsigned int replicas;
	TransportAddress dest_adr;
	std::vector<TransportAddress> send_list;

	replicas = getReplicaNr();

	createWritePkt(&write);

	for (i = 0 ; i < replicas ; i++)
	{
		//Duplicates the objects for sending
		go_dup = go->dup();
		write_dup = write->dup();

		if (i > 0) {
			go_dup->setType(REPLICA);
			write_dup->setName("replica_write");
		}
		else {
			//The default type is already ROOT
			write_dup->setName("write");
		}

		write_dup->addObject(go_dup);

		selectDestination(&dest_adr, send_list);
		write_dup->setDestinationAddress(dest_adr);

		send_list.push_back(dest_adr);

		//RECORD_STATS(numSent++; numPutSent++; numPutSuccess++);
		send(write_dup, "comms_gate$o");
	}

	delete(write);

	updateSuperPeerObjects(go->getObjectName(), (unsigned long)go->getSize(), send_list);
}

void GroupStorage::addPeers(cMessage *msg)
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
		cPacket *request_start = new cPacket("request_start");
		send(request_start, "to_upperTier");

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
		}
	}

	emit(groupSizeSignal, group_peers.size() + 1);	//The peer's perceived group size is one larger, because itself is part of the group it is in

	EV << "Added " << list_p->getPeer_listArraySize() << " new peers to the list.\n";
}

void GroupStorage::handleMessage(cMessage *msg)
{
	Packet *packet = check_and_cast<Packet *>(msg);

	if (packet->getPayloadType() == JOIN_ACCEPT)
	{
		addPeers(msg);
	}
	else if (packet->getPayloadType() == STORE_REQ)
	{
		GameObject *go = (GameObject *)msg->removeObject("GameObject");
		if (go == NULL)
			error("No object was attached to be stored in group storage");

		store(go);

		delete(go);		//Only duplicates of the game object are stored, so the original must be deleted
	}
	else error("Group storage received an unknown packet");

	delete(msg);
}

void GroupStorage::finishApp()
{
	/*cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

    simtime_t time = globalStatistics->calcMeasuredLifetime(communicator->getCreationTime());

    if (time >= GlobalStatistics::MIN_MEASURED) {
        // record scalar data
        globalStatistics->addStdDev("GroupStorage: Sent Total Messages/s",
                                    numSent / time);

        globalStatistics->addStdDev("GroupStorage: Sent PUT Messages/s",
                                    numPutSent / time);
        globalStatistics->addStdDev("GroupStorage: Failed PUT Requests/s",
                                    numPutError / time);
        globalStatistics->addStdDev("GroupStorage: Successful PUT Requests/s",
                                    numPutSuccess / time);

        if ((numGetSuccess + numGetError) > 0) {
            globalStatistics->addStdDev("GroupStorage: GET Success Ratio",
                                        (double) numGetSuccess
                                        / (double) (numGetSuccess + numGetError));
        }
    }*/
}
