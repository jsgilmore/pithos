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
	storage.setName("queue");
	take(&storage);

	//Register the signal that records the size of the group the peer is currently in
	groupSizeSignal = registerSignal("GroupSize");

	//Register the signal that records the number of times a put message failed
	//(This is now mostly handled by failure response messages to the calling module)
	groupSendFailSignal = registerSignal("GroupSendFail");

	//Register the signal that records the time at which the specific peer joined a group
	joinTimeSignal = registerSignal("JoinTime");

	//Initialise queue statistics collection
	qlenSignal = registerSignal("qlen");
	qsizeSignal = registerSignal("qsize");

	//Register the signals that record the time required for a specific type of object to be stored
	storeTimeSignal = registerSignal("storeTime");
	rootStoreTimeSignal = registerSignal("rootStoreTime");
	replicaStoreTimeSignal = registerSignal("replicaStoreTime");
	overlayStoreTimeSignal = registerSignal("overlayStoreTime");

	//Record the initial zero lengths of the object storage
	emit(qlenSignal, storage.length());
	emit(qsizeSignal, getStorageBytes());

	//Register the signals that record the number of different types of objects in storage
	overlayObjectsSignal = registerSignal("OverlayObject");
	rootObjectsSignal = registerSignal("RootObject");
	replicaObjectsSignal = registerSignal("ReplicaObject");

	//Read the IP address and port of the directory server from the NED file.
	strcpy(directory_ip, par("directory_ip"));
	directory_port = par("directory_port");

	//Set the event that will initiate the group join procedure.
	event = new cMessage("event");
	scheduleAt(simTime()+par("wait_time"), event);

	//Assign a random location to the peer in the virtual world
	latitude = uniform(0,100);		//Make this range changeable
	longitude = uniform(0,100);		//Make this range changeable

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

void GroupStorage::finish()
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

bool GroupStorage::hasSuperPeer()
{
	if (super_peer_address.isUnspecified())
	return false;
	else return true;
}

TransportAddress GroupStorage::getSuperPeerAddress()
{
	return super_peer_address;
}

int GroupStorage::getStorageFiles()
{
	return storage.getLength();
}

void GroupStorage::requestRetrieve(OverlayKeyPkt *retrieve_req)
{

}

void GroupStorage::updateSuperPeerObjects(GameObject *go, std::vector<TransportAddress> send_list)
{
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	PeerListPkt *objectAddPkt = new PeerListPkt();
	objectAddPkt->setByteLength(4+4+4+8+(send_list.size()*4));	//Source address, dest address, type, object name ID, storage peer addresses


	if (super_peer_address.isUnspecified())
	{
		//TODO: This error condition should be logged
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		delete(objectAddPkt);
		return;
	}

	objectAddPkt->setObjectName(go->getObjectName());
	objectAddPkt->setObjectSize(go->getSize());
	objectAddPkt->setObjectKey(go->getHash());
	objectAddPkt->setPayloadType(OBJECT_ADD);
	objectAddPkt->setSourceAddress(sourceAdr);
	objectAddPkt->setName("object_add");
	objectAddPkt->setDestinationAddress(super_peer_address);

	//Add the addresses of the other peers that have stored the data to the message
	for (unsigned int i = 0 ; i < send_list.size() ; i++)
	{
		PeerData peer_d;
		peer_d.setAddress(send_list.at(i));
		objectAddPkt->addToPeerList(peer_d);
	}

	send(objectAddPkt, "comms_gate$o");		//Set address
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

void GroupStorage::createWritePkt(ValuePkt **write, unsigned int rpcid)
{
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	//Create the packet that will house the game object
	(*write) = new ValuePkt();
	(*write)->setByteLength(4+4+4+8);	//Source address, dest address, type, object name ID and object size
	(*write)->setPayloadType(WRITE);
	(*write)->setValue(rpcid);
	(*write)->setSourceAddress(sourceAdr);
}

int GroupStorage::getReplicaNr(unsigned int rpcid)
{
	unsigned int replicas = par("replicas");

	//This ensures that an infinite while loop situation will never occur, but it also constrains the number of replicas to the number of known nodes
	if (replicas > group_peers.size())
	{
		unsigned int i;
		ResponsePkt *response = new ResponsePkt();

		response->setResponseType(GROUP_PUT);
		response->setPayloadType(RESPONSE);
		response->setIsSuccess(false);
		response->setRpcid(rpcid);		//This allows the higher layer to know which RPC call is being acknowledged.

		//Send one failure response packet for each replica that cannot be stored
		for (i = 0 ; i < replicas - group_peers.size() - 1 ; i++)
			send(response->dup(), "read");
		send(response, "read");

		emit(groupSendFailSignal, replicas - group_peers.size());
		//RECORD_STATS(numPutError++);
		replicas = group_peers.size();
	}

	return replicas;
}

void GroupStorage::send_forstore(GameObject *go, unsigned int rpcid)
{
	unsigned int i;
	simtime_t sendDelay;
	GameObject *go_dup;
	ValuePkt *write = NULL;
	ValuePkt *write_dup;

	unsigned int replicas;
	TransportAddress dest_adr;
	std::vector<TransportAddress> send_list;

	replicas = getReplicaNr(rpcid);

	createWritePkt(&write, rpcid);

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

	updateSuperPeerObjects(go, send_list);
}

void GroupStorage::respond_toUpper(cMessage *msg)
{
	//ResponsePkt *response = check_and_cast<ResponsePkt *>(msg);

	//TODO: Perhaps collect some statistics at this point, that is not worth collecting in the higher layer.
	/*if (response->getIsSuccess()) {
		RECORD_STATS(numPutSuccess++);
		RECORD_STATS(globalStatistics->addStdDev("GroupStorage: PUT Latency (s)", SIMTIME_DBL(simTime() - context->requestTime)));
	} else {
		RECORD_STATS(numPutError++);
	}*/

	send(msg, "read");
}

void GroupStorage::sendUDPResponse(cMessage *msg)
{
	ResponsePkt *response = new ResponsePkt();
	ValuePkt *store_req = check_and_cast<ValuePkt *>(msg);

	response->setSourceAddress(store_req->getDestinationAddress());
	response->setDestinationAddress(store_req->getSourceAddress());
	response->setResponseType(GROUP_PUT);
	response->setPayloadType(RESPONSE);
	response->setIsSuccess(true);
	response->setRpcid(store_req->getValue());		//This allows the higher layer to know which RPC call is being acknowledged.
	send(response, "comms_gate$o");
}

int GroupStorage::getStorageBytes()
{
	int i;
	int total_size = 0;

	//This is inefficient, since a sequential search will be done for every element in the queue.
	//TODO: The "forEachChild" method should rather be implemented with an appropriate visitor class.
	for (i = 0 ; i < storage.getLength() ; i++)
	{
		total_size += ((GameObject *)storage.get(i))->getSize();
	}

	return total_size;
}

void GroupStorage::store(cMessage *msg)
{
	simtime_t delay;

	if (!(msg->hasObject("GameObject")))
		error("Storage received a message with no game object attached");

	GameObject *go = (GameObject *)msg->removeObject("GameObject");

	if (go->getType() == ROOT)
		EV << getName() << " " << getIndex() << " received root Game Object of size " << go->getSize() << "\n";
	else if (go->getType() == REPLICA)
		EV << getName() << " " << getIndex() << " received replica Game Object of size " << go->getSize() << "\n";

	delay = simTime() - go->getCreationTime();

	EV << getName() << " " << getIndex() << " received write command of size " << go->getSize() << " with delay " << go->getCreationTime() << "\n";

	emit(storeTimeSignal, delay);

	storage.insert(go);

	emit(qlenSignal, storage.length());
	emit(qsizeSignal, getStorageBytes());

	if (go->getType() == ROOT)
	{
		emit(rootObjectsSignal, 1);
		emit(rootStoreTimeSignal, delay);
	}
	else if (go->getType() == REPLICA)
	{
		emit(replicaObjectsSignal, 1);
		emit(replicaStoreTimeSignal, delay);
	}
	else if (go->getType() == OVERLAY)
	{
		emit(overlayObjectsSignal, 1);
		emit(overlayStoreTimeSignal, delay);
	}
	else error("The game object type was incorrectly set");

	sendUDPResponse(msg);
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

void GroupStorage::joinRequest(const TransportAddress &dest_adr)
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

void GroupStorage::addAndJoinSuperPeer(Packet *packet)
{
	char err_str[50];

	if (packet->getPayloadType() == INFORM)
	{
		bootstrapPkt *boot_p = check_and_cast<bootstrapPkt *>(packet);

		super_peer_address = boot_p->getSuperPeerAdr();
		EV << "A new super peer has been identified at " << super_peer_address << endl;

		cancelAndDelete(event);		//We've received the data from the directory server, so we can stop harassing them now

		joinRequest(super_peer_address);
	}
	else {
		sprintf(err_str, "Illegal P2P message received (%s)", packet->getName());
		error (err_str);
	}
}

void GroupStorage::handleMessage(cMessage *msg)
{
	if (msg == event)
	{
		//For the first join request, a request is sent to the well known directory server
		IPAddress dest_ip(directory_ip);
		TransportAddress destAdr(dest_ip, directory_port);

		joinRequest(destAdr);

		scheduleAt(simTime()+1, event);		//TODO: make the 1 second wait time a configuration variable that may be set

	} else {

		Packet *packet = check_and_cast<Packet *>(msg);

		if (packet->getPayloadType() == INFORM)
		{
			//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
			addAndJoinSuperPeer(packet);
			delete(msg);
		} else if (packet->getPayloadType() == JOIN_ACCEPT)
		{
			addPeers(msg);
			delete(msg);
		}
		else if (packet->getPayloadType() == WRITE)
		{
			store(msg);
			delete(msg);
		}
		else if (packet->getPayloadType() == RESPONSE)
		{
			respond_toUpper(msg);
		}
		else if (packet->getPayloadType() == STORE_REQ)
		{
			ValuePkt *store_req = check_and_cast<ValuePkt *>(msg);

			GameObject *go = (GameObject *)msg->removeObject("GameObject");
			if (go == NULL)
				error("No object was attached to be stored in group storage");

			send_forstore(go, store_req->getValue());

			delete(go);		//Only duplicates of the game object are stored, so the original must be deleted
			delete(msg);
		} else if (packet->getPayloadType() == RETRIEVE_REQ)
		{
			OverlayKeyPkt *retrieve_req = check_and_cast<OverlayKeyPkt *>(msg);

			requestRetrieve(retrieve_req);

			delete(msg);	//The group request path should start here
		}
		else error("Group storage received an unknown packet");
	}
}
