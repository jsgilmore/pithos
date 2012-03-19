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
	event = NULL;
}

GroupStorage::~GroupStorage()
{
	PendingRequests::iterator requests_it;
	std::vector<ResponseTimeoutEvent *>::iterator timeout_it;

	cancelAndDelete(event);

	for (requests_it = pendingRequests.begin(); requests_it != pendingRequests.end(); requests_it++)
	{
		for (timeout_it = requests_it->second.timeouts.begin() ; timeout_it != requests_it->second.timeouts.end() ; timeout_it++)
		{
			cancelAndDelete(*timeout_it);
		}
		requests_it->second.timeouts.clear();
	}

	pendingRequests.clear();

	storage_map.clear();
}

void GroupStorage::initialize()
{
	//Read the IP address and port of the directory server from the NED file.
	strcpy(directory_ip, par("directory_ip"));
	directory_port = par("directory_port");

	requestTimeout = par("requestTimeout");

	//Link this node's group ledger with this group storage module
	cModule *groupLedgerModule = getParentModule()->getSubmodule("group_ledger");
	group_ledger = check_and_cast<GroupLedger *>(groupLedgerModule);
	if (group_ledger == NULL) {
		throw cRuntimeError("GroupStorage::initializeApp(): Group ledger module not found!");
	}

	event = new cMessage();	//This is the join retry timer.

	globalStatistics = GlobalStatisticsAccess().get();

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
	//Record the initial zero lengths of the object storage
	emit(qlenSignal, storage_map.size());
	emit(qsizeSignal, getStorageBytes());
	//Register the signals that record the number of different types of objects in storage
	objectsSignal = registerSignal("Object");

	// statistics
	numSent = 0;
	numPutSent = 0;
	numPutError = 0;
	numPutSuccess = 0;
	numGetSent = 0;
	numGetError = 0;
	numGetSuccess = 0;

	//initRpcs();
	WATCH(numSent);
	WATCH(numPutSent);
	WATCH(numPutError);
	WATCH(numPutSuccess);
	WATCH(numGetSent);
	WATCH(numGetError);
	WATCH(numGetSuccess);

	WATCH_MAP(storage_map);
}

void GroupStorage::finish()
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

    simtime_t time = globalStatistics->calcMeasuredLifetime(communicator->getCreationTime());

    if (time >= GlobalStatistics::MIN_MEASURED) {
        // record scalar data
        globalStatistics->addStdDev("GroupStorage: Sent Total Messages", numSent);

        globalStatistics->addStdDev("GroupStorage: Sent PUT Messages", numPutSent);
        globalStatistics->addStdDev("GroupStorage: Failed PUT Requests", numPutError);
        globalStatistics->addStdDev("GroupStorage: Successful PUT Requests", numPutSuccess);

        globalStatistics->addStdDev("GroupStorage: Sent GET Messages", numGetSent);
		globalStatistics->addStdDev("GroupStorage: Failed GET Requests", numGetError);
		globalStatistics->addStdDev("GroupStorage: Successful GET Requests", numGetSuccess);

        if ((numGetSuccess + numGetError) > 0) {
            globalStatistics->addStdDev("GroupStorage: GET Success Ratio", (double) numGetSuccess / (double) (numGetSuccess + numGetError));
        }
    }
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
	return storage_map.size();
}

void GroupStorage::createResponseMsg(ResponsePkt **response, int responseType, unsigned int rpcid, bool isSuccess, GameObject object)
{
	//Create the packet that will house the game object
	(*response) = new ResponsePkt();
	(*response)->setGroupAddress(super_peer_address);	//This records the group (super peer) address for use by PithosTestApp to generate group requests
	(*response)->setResponseType(responseType);
	(*response)->setPayloadType(RESPONSE);
	(*response)->setIsSuccess(isSuccess);
	(*response)->setRpcid(rpcid);		//This allows the higher layer to know which RPC call is being acknowledged.

	if (object != GameObject::UNSPECIFIED_OBJECT)
	{
		EV << "[GroupStorage] returning result: " << object << endl;
		GameObject *object_ptr =  new GameObject(object);
		(*response)->addObject(object_ptr);

		//Response packet + object size
		(*response)->setByteLength(RESPONSE_PKT_SIZE + object.getSize());
	} else {
		//Packet + ResponseType + isSuccess + RPCID
		(*response)->setByteLength(RESPONSE_PKT_SIZE);
	}
}

void GroupStorage::sendUDPResponse(TransportAddress src_adr, TransportAddress dest_adr, int responseType, unsigned int rpcid, bool isSuccess, GameObject object)
{
	ResponsePkt *response;

	createResponseMsg(&response, responseType, rpcid, isSuccess, object);

	response->setSourceAddress(src_adr);
	response->setDestinationAddress(dest_adr);

	send(response, "comms_gate$o");
}

void GroupStorage::sendUpperResponse(int responseType, unsigned int rpcid, bool isSuccess, GameObject object)
{
	ResponsePkt *response;

	createResponseMsg(&response, responseType, rpcid, isSuccess, object);

	send(response, "read");
}

void GroupStorage::forwardRequest(OverlayKeyPkt *retrieve_req)
{
	PeerData container_peer;
	int rpcid = retrieve_req->getValue();

	//TODO: This check can later be removed if a partially connected group is required with recursive routing within the group.
	//Such a grouping could have nodes connected to a certain percentage of other nodes, reducing group bandwidth usage and only slightly increasing latency.
	if (retrieve_req->getHops() > 0)
		error("[GroupStorage]: Object not found on destination node in group.");

	RECORD_STATS(numSent++; numGetSent++);

	container_peer = group_ledger->getRandomPeer(retrieve_req->getKey());

	//Send a retrieve request to the group peer storing the object
	retrieve_req->setDestinationAddress(container_peer.getAddress());
	retrieve_req->setGroupAddress(super_peer_address);

	retrieve_req->setHops(retrieve_req->getHops()+1);

	send(retrieve_req, "comms_gate$o");

	//Create and schedule request timeout timer
	ResponseTimeoutEvent *timeout = new ResponseTimeoutEvent("timeout");
	timeout->setRpcid(rpcid);
	timeout->setPeerData(container_peer);
	scheduleAt(simTime()+requestTimeout, timeout);

	//Insert timer into list of pending requests
	PendingRequestsEntry entry;
	entry.numGetSent = 1;	//Only one message is sent per get request
	entry.responseType = GROUP_GET;
	entry.timeouts.push_back(timeout);

	pendingRequests.insert(std::make_pair(rpcid, entry));
}

bool GroupStorage::handleMissingObject(OverlayKeyPkt *retrieve_req)
{
	OverlayKey key = retrieve_req->getKey();
	int rpcid = retrieve_req->getValue();

	//Is this object actually stored in this group
	if (!(group_ledger->isObjectInGroup(key)))
	{
		RECORD_STATS(numGetError++);

		if (retrieve_req->getSourceAddress() == retrieve_req->getDestinationAddress())
		{
			//If the object is not stored in the group, send a failure response to the higher layer
			sendUpperResponse(GROUP_GET, rpcid, false);
			delete(retrieve_req);
			return true;
		} else {
			//If the object is not stored in the group, send a failure response to the higher layer
			sendUDPResponse(retrieve_req->getDestinationAddress(), retrieve_req->getSourceAddress(), GROUP_GET, rpcid, false);
			delete(retrieve_req);
			return true;
		}
	}
	return false;
}

bool GroupStorage::retrieveLocally(OverlayKeyPkt *retrieve_req)
{
	StorageMap::iterator storage_map_it = storage_map.find(retrieve_req->getKey());
	int rpcid = retrieve_req->getValue();

	if (storage_map_it != storage_map.end())
	{
		//If the object is stored in the group, check whether the object is on the same peer that sent the request
		if (retrieve_req->getSourceAddress() == retrieve_req->getDestinationAddress())
		{
			//If the source and destination addresses are the same it means the request comes from the higher layer and not another peer
			//The object is therefore on the requesting peer itself and we can just reply with the object to the higher layer directly
			RECORD_STATS(numGetSuccess++);
			//If the object is stored in local storage, send it to the upper layer without requesting from the group
			sendUpperResponse(GROUP_GET, rpcid, true, storage_map_it->second);
			delete(retrieve_req);
			return true;
		} else {
			//If the object is stored on this peer, but another peer sent the request, send a UDP response with the object
			sendUDPResponse(retrieve_req->getDestinationAddress(), retrieve_req->getSourceAddress(), GROUP_GET, rpcid, true, storage_map_it->second);
			delete(retrieve_req);
			return true;
		}
	}

	return false;
}

//This is kind of a recursive distributed network function. Keep that in mind when figuring out the code+comments
void GroupStorage::requestRetrieve(OverlayKeyPkt *retrieve_req)
{
	unsigned int rpcid = retrieve_req->getValue();
	bool isSuccess;

	//This can happen if this peer switched groups, after being selected to retrieve a packet
	//It should also be noted that this if and the next is required, otherwise an upper response will be sent, instead of a UDP response.
	//The first request to come from the upper layer will have an unspecified group address and has to be ignored. An illegal compare eror wil be generated if it's not the first hop
	if (!(retrieve_req->getGroupAddress().isUnspecified()) || (retrieve_req->getHops() != 0))
	{
		if (retrieve_req->getGroupAddress() != super_peer_address)
		{
			sendUDPResponse(retrieve_req->getDestinationAddress(), retrieve_req->getSourceAddress(), GROUP_GET, rpcid, false);
			delete(retrieve_req);
			return;
		}
	}

	isSuccess = retrieveLocally(retrieve_req);
	if (isSuccess) return;

	isSuccess = handleMissingObject(retrieve_req);
	if (isSuccess) return;

	forwardRequest(retrieve_req);
}

void GroupStorage::updatePeerObjects(GameObject go)
{
	PeerListPkt *objectAddPkt = new PeerListPkt();
	objectAddPkt->setByteLength(PEERLIST_PKT_SIZE(PEERDATA_SIZE));

	if (super_peer_address.isUnspecified())
	{
		//TODO: This error condition should be logged
		EV << "No super peer has been identified. The group object will not be stored on this peer\n";
		delete(objectAddPkt);
		return;
	}

	objectAddPkt->setObjectData(ObjectData(go.getObjectName(), go.getSize(), go.getHash(), go.getCreationTime(), go.getTTL()));
	objectAddPkt->setPayloadType(OBJECT_ADD);
	objectAddPkt->setSourceAddress(this_address);
	objectAddPkt->setName("object_add");
	objectAddPkt->setGroupAddress(super_peer_address);

	objectAddPkt->addToPeerList(PeerData(this_address));

	//Inform all group peers about the new object and where it is stored
	for (unsigned int i = 0 ; i < group_ledger->getGroupSize() ; i++)
	{
		TransportAddress dest_adr = (*(group_ledger->getPeerPtr(i))).getAddress();
		objectAddPkt->setDestinationAddress(dest_adr);
		send(objectAddPkt->dup(), "comms_gate$o");		//Set address
	}

	//Inform the super peer of the new object and the peer that contains it
	//The super peer object type must be different to the group object type for communicator routing purposes
	objectAddPkt->setPayloadType(SP_OBJECT_ADD);
	objectAddPkt->setDestinationAddress(super_peer_address);
	send(objectAddPkt, "comms_gate$o");		//Set address
}

PeerData GroupStorage::selectDestination(std::vector<TransportAddress> send_list)
{
	unsigned int j;
	bool original_address = false;
	PeerData peerData;

	while(!original_address)
	{
		peerData = group_ledger->getRandomPeer();		//Choose a uniform random peer in the group for the destination

		//Check all previous chosen addresses to determine whether this address is unique
		original_address = true;

		for (j = 0 ; j < send_list.size() ; j++)
		{
			if (send_list.at(j) == peerData.getAddress())
				original_address = false;
		}
	}

	return peerData;
}

void GroupStorage::createWritePkt(ValuePkt **write, unsigned int rpcid)
{
	//Create the packet that will house the game object
	(*write) = new ValuePkt("write");
	(*write)->setByteLength(VALUE_PKT_SIZE);
	(*write)->setPayloadType(WRITE);
	(*write)->setValue(rpcid);
	(*write)->setSourceAddress(this_address);
	(*write)->setGroupAddress(super_peer_address);
}

int GroupStorage::getReplicaNr(unsigned int rpcid)
{
	unsigned int replicas = par("replicas");

	//This ensures that an infinite while loop situation will never occur, but it also constrains the number of replicas to the number of known nodes
	if (replicas > group_ledger->getGroupSize())
	{
		unsigned int i;
		ResponsePkt *response = new ResponsePkt();

		response->setResponseType(GROUP_PUT);
		response->setPayloadType(RESPONSE);
		response->setIsSuccess(false);
		response->setRpcid(rpcid);		//This allows the higher layer to know which RPC call is being acknowledged.
		//This packet is sent internally, so no size is required

		//Send one failure response packet for each replica that cannot be stored
		for (i = 0 ; i < replicas - group_ledger->getGroupSize() ; i++)
			send(response->dup(), "read");

		emit(groupSendFailSignal, replicas - group_ledger->getGroupSize());
		RECORD_STATS(numPutError++);
		replicas = group_ledger->getGroupSize();

		//If there is only one peer available, the object will be lost as soon as that peer leaves the group,
		//since there will be no other peers that can be used for replication. We therefore report put failure if no replication can be done.
		//This only happens with new groups, where there are few peers in the group
		if (replicas == 1)
		{
			send(response, "read");
			return 0;
		}
		else delete(response);
	}

	return replicas;
}

void GroupStorage::send_forstore(ValuePkt *store_req)
{
	unsigned int i;
	simtime_t sendDelay;
	GameObject *go_dup;
	ValuePkt *write = NULL;
	ValuePkt *write_dup;

	unsigned int replicas;
	std::vector<TransportAddress> send_list;

	PendingRequestsEntry entry;
	ResponseTimeoutEvent *timeout;
	PeerData destAdr;

	int rpcid = store_req->getValue();

	replicas = getReplicaNr(rpcid);
	if (replicas == 0)
		return;

	createWritePkt(&write, rpcid);

	GameObject *go = (GameObject *)store_req->removeObject("GameObject");
	if (go == NULL)
		error("No object was attached to be stored in group storage");

	//std::cout << "Inserting pending put request with rpcid: " << rpcid << endl;
	//std::cout << simTime() << ": Inserting object (" << go->getObjectName() << ") with " << replicas << " replicas.\n";

	//Add a new request for which at least one response is required
	entry.numPutSent = replicas;
	entry.responseType = GROUP_PUT;

	for (i = 0 ; i < replicas ; i++)
	{
		//Duplicates the objects for sending
		go_dup = go->dup();
		write_dup = write->dup();

		write_dup->addObject(go_dup);

		destAdr = selectDestination(send_list);
		write_dup->setDestinationAddress(destAdr.getAddress());

		send_list.push_back(destAdr.getAddress());

		RECORD_STATS(numSent++; numPutSent++);
		send(write_dup, "comms_gate$o");

		timeout = new ResponseTimeoutEvent("timeout");
		timeout->setRpcid(rpcid);
		timeout->setPeerData(destAdr);
		scheduleAt(simTime()+requestTimeout, timeout);

		entry.timeouts.push_back(timeout);
	}

	pendingRequests.insert(std::make_pair(rpcid, entry));

	delete(go);		//Only duplicates of the game object are stored, so the original must be deleted
	delete(write);
}

void GroupStorage::handleResponse(PendingRequests::iterator it, ResponsePkt *response)
{
	if (response->getResponseType() == GROUP_PUT)
	{
		if (response->getIsSuccess())
		{
			it->second.numGroupPutSucceeded++;
			RECORD_STATS(numPutSuccess++);
		}
		else {
			it->second.numGroupPutFailed++;
			RECORD_STATS(numPutError++);		//These errors are currently all caused by an insufficient number of group peers to store all replicas.
		}

		if (it->second.numGroupPutSucceeded + it->second.numGroupPutFailed == it->second.numPutSent)
		{
			pendingRequests.erase(it);
		}
	} else if (response->getResponseType() == GROUP_GET)
	{
		if (response->getIsSuccess())
		{
			it->second.numGroupGetSucceeded++;
			RECORD_STATS(numGetSuccess++);
		}
		else {
			it->second.numGroupGetFailed++;
			RECORD_STATS(numGetError++);
		}

		pendingRequests.erase(it);
	} else error("Unknown response type received");
}

PeerData GroupStorage::cancelRequestTimer(PendingRequests::iterator it, TransportAddress source_address)
{
	bool found = false;
	ResponseTimeoutEvent * timeout;
	std::vector<ResponseTimeoutEvent *>::iterator timeout_it;
	PeerData peerData;

	//std::cout << "Received response timeout address: " << response->getSourceAddress() << endl;
	//For debugging purposes only
	/*if (it->second.numPutSent > 0)
		std::cout << "\nResponse address in received PUT message: " << response->getSourceAddress() << endl;
	else if (it->second.numGetSent > 0)
		std::cout << "\nResponse address in received GET message: " << response->getSourceAddress() << endl;
	else	error("No puts or gets registered in pending requests record.");*/

	//Cancel the timeout for the responding peer
	for (timeout_it = it->second.timeouts.begin() ; timeout_it != it->second.timeouts.end() ; timeout_it++)
	{
		timeout = *timeout_it;
		peerData = timeout->getPeerData();

		//std::cout << "Response address in timeout vector: " << address << endl;

		//timeout_it is an iterator to a pointer, so it has to be dereferenced once to get to the ResponseTimeoutEvent pointer
		if (peerData.getAddress() == source_address)
		{
			cancelAndDelete(timeout);
			it->second.timeouts.erase(timeout_it);
			found = true;
			break;
		}
	}

	if (!found)
		error("No timeout found for response message from peer.");

	return peerData;
}

void GroupStorage::respond_toUpper(cMessage *msg)
{
	ResponsePkt *response = check_and_cast<ResponsePkt *>(msg);
	PeerData peerData;

	PendingRequests::iterator it = pendingRequests.find(response->getRpcid());

	if (it != pendingRequests.end()) // unknown request or request for already erased call
	{
		peerData = cancelRequestTimer(it, response->getSourceAddress());

		//TODO: If a response has been received for a request that has already timed out, that response should not be forwarded to the upper layer.

		/**
		 * If a response is received from a peer outside the current group, remove that peer from the group ledger
		 * This can occur when a peer leaves the group, just as this peer joins it. The joining peer is informed of
		 * the leaving peer by an object that is stored on that peer.
		 * The leaving peer does not yet know about the joining peer, so cannot inform the joining peer that the leaving peer is leaving the group.
		 */
		if (response->getGroupAddress() != super_peer_address)
		{
			//PeerData wil here be equal to the peer data found in the appropriate timeout
			group_ledger->removePeer(peerData);
			//LastPeerLeft should not be updated here, since it might have been this peer that left
		}

		//TODO: Record the group put latency (This will merely require that the response packet be expanded with the initiation time of the request)

		handleResponse(it, response);

	} /*else {
		//std::cout << "[" << simTime() << ":" << this_address <<"]: Timeout with unknown RPCID received (" << response->getRpcid() << ")\n";
	}*/

	send(msg, "read");
}

int GroupStorage::getStorageBytes()
{
	int total_size = 0;

	StorageMap::iterator it;

	for (it = storage_map.begin() ; it != storage_map.end() ; it++)
	{
		total_size += it->second.getSize();
	}

	return total_size;
}

void GroupStorage::store(Packet *pkt)
{
	std::pair<StorageMap::iterator,bool> ret;

	//This happens when a group peer changes groups, after being selected to store a file
	if (pkt->getGroupAddress() != super_peer_address)
	{
		if (pkt->getPayloadType() == WRITE)
		{
			//We can also receive a replicate packet, which does not expect a response
			ValuePkt *value_pkt = check_and_cast<ValuePkt *>(pkt);
			sendUDPResponse(value_pkt->getDestinationAddress(), value_pkt->getSourceAddress(), GROUP_PUT, value_pkt->getValue(), false);
			return;
		} else return;
	}

	if (!(pkt->hasObject("GameObject")))
		error("[GroupStorage::store]: Storage received a message with no game object attached");

	//We're not removing the object here, because we're only using the value and then the object is deleted with the message.
	GameObject *go = (GameObject *)pkt->getObject("GameObject");
	go->setGroupAddress(super_peer_address);

	//std::cout << "[GroupStorageTarget] Stored object with key " << go->getHash() << endl;
	EV << getName() << " " << getIndex() << " received Game Object of size " << go->getSize() << "\n";
	EV << getName() << " " << getIndex() << " received write command of size " << go->getSize() << " with delay " << go->getCreationTime() << "\n";

	ret = storage_map.insert(std::make_pair(go->getHash(), *go));
	//Ensure that a duplicate key wasn't inserted
	if (ret.second == false)
		return;
		//error("[GroupStorage::store]: Duplicate key inserted into storage.");

	emit(qlenSignal, storage_map.size());
	emit(qsizeSignal, getStorageBytes());
	emit(objectsSignal, 1);

	//Schedule the object to be removed when its TTL expires.
	ObjectTTLTimer* timer = new ObjectTTLTimer();
	timer->setKey(go->getHash());
	scheduleAt(go->getCreationTime() + go->getTTL(), timer);

	if (pkt->getPayloadType() == WRITE)
	{
		//We can also receive a replicate packet, which does not expect a response
		ValuePkt *value_pkt = check_and_cast<ValuePkt *>(pkt);
		sendUDPResponse(value_pkt->getDestinationAddress(), value_pkt->getSourceAddress(), GROUP_PUT, value_pkt->getValue(), true);
	}

	updatePeerObjects(*go);
}

void GroupStorage::addToGroup(cMessage *msg)
{
	PeerListPkt *list_p = check_and_cast<PeerListPkt *>(msg);
	PeerData peer_dat;
	ObjectData object_dat;
	simtime_t joinTime = simTime();

	//If a packet was received from another group, ignore it.
	if (list_p->getGroupAddress() != super_peer_address)
		return;

	//If we didn't know of any peers in our group and we've now been informed of some, inform the game module to start producing requests
	//This is also the time when we record that we've successfully joined a group (When we've joined a super peer and we know of other peers in the group).
	if ((group_ledger->getGroupSize() == 0) && (list_p->getPeer_listArraySize() > 0))
	{
		AddressPkt *request_start = new AddressPkt("request_start");
		request_start->setAddress(super_peer_address);
		request_start->setByteLength(ADDRESS_SIZE);	//The super peer IP
		send(request_start, "to_upperTier");

		emit(joinTimeSignal, joinTime);
	}

	object_dat = list_p->getObjectData();

	//std::cout << "Peer list array size: " << list_p->getPeer_listArraySize() << endl;

	for (unsigned int i = 0 ; i < list_p->getPeer_listArraySize() ; i++)
	{
		peer_dat = list_p->getPeer_list(i);

		//Check whether this peer didn't just leave the group and this message is in fact a delayed out dated message
		if ((lastPeerLeft.getAddress().isUnspecified()) || (peer_dat != lastPeerLeft))
		{
			if ((list_p->getObjectData()).isUnspecified())
				group_ledger->addPeer(peer_dat);
			else {
					group_ledger->addObject(object_dat, peer_dat);
			}
		} else {
			//std::cout << "[" << simTime() << ":" << this_address <<"]: Unsuccessful add, peer was last peer that left (" << peer_dat.getAddress() << ")\n";
		}

	}

	emit(groupSizeSignal, group_ledger->getGroupSize() + 1);	//The peer's group size is one peer larger than its perceived group size, because itself is part of the group it is in

	EV << "Added " << list_p->getPeer_listArraySize() << " new peers to the list.\n";
}

void GroupStorage::joinRequest(const TransportAddress &dest_adr)
{
	if (dest_adr.isUnspecified())
		error("Destination address is unspecified when requesting a join.\n");

	bootstrapPkt *boot_p = new bootstrapPkt();
	boot_p->setSourceAddress(this_address);
	boot_p->setDestinationAddress(dest_adr);
	boot_p->setPayloadType(JOIN_REQ);
	boot_p->setName("join_req");
	boot_p->setLatitude(latitude);
	boot_p->setLongitude(longitude);
	boot_p->setByteLength(BOOTSTRAP_PKT_SIZE);	//Src IP as #, Dest IP as #, Type, Lat, Long

	send(boot_p, "comms_gate$o");
}

void GroupStorage::leaveGroup()
{
	group_ledger->removePeer(PeerData(this_address));

	peerLeftInform(PeerData(this_address));

	group_ledger->recordAndClear();
	storage_map.clear();

	lastPeerLeft = PeerData();	//Sets the transport address to unspecified for the new group
}

void GroupStorage::addAndJoinSuperPeer(Packet *packet)
{
	bootstrapPkt *boot_p = check_and_cast<bootstrapPkt *>(packet);

	//This event should only repeat while we don't receive responses from the directory server.
	//This is on account of there not being any super peers known to the server. When we do receive a message, we cancel the event.
	cancelEvent(event);		//We've received the data from the directory server, so we can stop harassing them now
	//This event is only canceled, and not deleted here, because the packet could have been delayed,
	//which will cause a second "INFORM" packet to arrive. Deleting the already created event will cause a segfault.

	if (!(super_peer_address.isUnspecified()))
	{
		//If the super peer address provided by the directory server remains the same, don't do anything
		if (super_peer_address == boot_p->getSuperPeerAdr())
			return;

		leaveGroup();
	}


	super_peer_address = boot_p->getSuperPeerAdr();
	//std::cout << "Adding new group address: " << super_peer_address << endl;

	if (super_peer_address.isUnspecified())
		error("The address is unspecified.");

	EV << "A new super peer has been identified at " << super_peer_address << endl;

	joinRequest(super_peer_address);
}

void GroupStorage::handleLeftPeer(PeerDataPkt *peer_data_pkt)
{
	//If a packet was received from another group, ignore it.
	if (peer_data_pkt->getGroupAddress() != super_peer_address)
	{
		return;
	}

	group_ledger->removePeer(peer_data_pkt->getPeerData());

	//The peer's perceived group size is one larger, because itself is part of the group it is in
	emit(groupSizeSignal, group_ledger->getGroupSize() + 1);

	//Record the data of the last peer that left, in case we get an outdated object add message from that peer
	lastPeerLeft = peer_data_pkt->getPeerData();
}

void GroupStorage::replicate(ReplicationReqPkt *replicate_pkt)
{

	PeerData peer_data;
	StorageMap::iterator storage_it;
	GameObject *go;
	std::set<TransportAddress> selected_peers;
	std::set<TransportAddress>::iterator selected_it;

	if (replicate_pkt->getGroupAddress() != super_peer_address)
	{
		delete(replicate_pkt);
		return;
	}

	int replicas = par("replicas");

	ObjectData object_data = replicate_pkt->getObjectData();

	//std::cout << "[" << simTime() << ":" << this_address << "]: Replicating object: " << object_data.getObjectName() << endl;

	for (int i = 0 ; i < replicate_pkt->getReplicaDiff() ; i++)
	{
		bool objectIsOnPeer = true;
		int retries = 0;

		//Find a group peer that does not already contain the object
		while(objectIsOnPeer)
		{
			peer_data = group_ledger->getRandomPeer();
			selected_it = selected_peers.find(peer_data.getAddress());

			if (!(group_ledger->isObjectOnPeer(object_data, peer_data)) || (selected_it == selected_peers.end()))
			{
				objectIsOnPeer = false;
			} else retries++;

			//If we've retried for more times than there are replicas, we give up (because our choices are random, there might still be a unique peer).
			//TODO: The number of time should be matched against the actual number of peers present.
			if (retries == replicas)
				return;
		}
		selected_peers.insert(peer_data.getAddress());

		//Retrieve the object from local storage
		storage_it = storage_map.find(object_data.getKey());
		if (storage_it == storage_map.end())
				error("[replicate]: Object could not be found in local storage.");

		go = new GameObject(storage_it->second);	//A dynamic game object is required to add to an Omnet message

		//std::cout << "Replicating object (" << go->getObjectName()  << ") from " << this_address << " on " << peer_data.getAddress() << endl;

		//Create the packet that will house the game object
		Packet *write = new Packet("replicate");
		write->setByteLength(PKT_SIZE + go->getSize());
		write->setPayloadType(REPLICATE);
		write->setSourceAddress(this_address);
		write->setDestinationAddress(peer_data.getAddress());
		write->setGroupAddress(super_peer_address);
		write->addObject(go);

		send(write, "comms_gate$o");
	}
}

void GroupStorage::handlePacket(Packet *packet)
{

	if (packet->getPayloadType() == INFORM)
	{
		//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
		addAndJoinSuperPeer(packet);
		delete(packet);
	} else if (packet->getPayloadType() == OBJECT_ADD)
	{
		addToGroup(packet);
		delete(packet);
	}else if (packet->getPayloadType() == JOIN_ACCEPT)
	{
		addToGroup(packet);
		delete(packet);
	}else if (packet->getPayloadType() == PEER_JOIN)
	{
		addToGroup(packet);
		delete(packet);
	} else if (packet->getPayloadType() == WRITE)
	{
		store(packet);
		delete(packet);
	} else if (packet->getPayloadType() == REPLICATE)
	{
		store(packet);
		delete(packet);
	} else if (packet->getPayloadType() == RESPONSE)
	{
		respond_toUpper(packet);
	} else if (packet->getPayloadType() == STORE_REQ)
	{
		ValuePkt *store_req = check_and_cast<ValuePkt *>(packet);

		send_forstore(store_req);
		delete(packet);
	} else if (packet->getPayloadType() == RETRIEVE_REQ)
	{
		OverlayKeyPkt *retrieve_req = check_and_cast<OverlayKeyPkt *>(packet);

		requestRetrieve(retrieve_req);
	} else if (packet->getPayloadType() == REPLICATION_REQ)
	{
		ReplicationReqPkt *replicate_pkt = check_and_cast<ReplicationReqPkt *>(packet);

		replicate(replicate_pkt);
		delete(packet);
	} else if (packet->getPayloadType() == PEER_LEFT)
	{
		PeerDataPkt *peer_data_pkt = check_and_cast<PeerDataPkt *>(packet);

		handleLeftPeer(peer_data_pkt);
		delete(packet);
	}
	else error("Group storage received an unknown packet");
}

void GroupStorage::peerLeftInform(PeerData peerData)
{
	std::vector<PeerDataPtr>::iterator it;
	PeerDataPkt *pkt = new PeerDataPkt("peerLeft");

	pkt->setSourceAddress(this_address);
	pkt->setGroupAddress(super_peer_address);
	pkt->setPayloadType(PEER_LEFT);
	pkt->setPeerData(peerData);
	pkt->setByteLength(PEERDATA_PKT_SIZE);

	std::ostringstream msg;
	msg << "[" << this_address << "]: Leaving peer group size\n";

	RECORD_STATS(globalStatistics->recordOutVector(msg.str().c_str(), group_ledger->getGroupSize()));

	for (unsigned int i = 0 ; i < group_ledger->getGroupSize() ; i++)
	{
		//Dereference it to get PeerDataPtr and use -> operator to get PeerData
		pkt->setDestinationAddress(group_ledger->getPeerPtr(i)->getAddress());

		send(pkt->dup(), "comms_gate$o");
	}

	pkt->setDestinationAddress(super_peer_address);
	pkt->setPayloadType(SP_PEER_LEFT);
	send(pkt, "comms_gate$o");
}

void GroupStorage::handleTimeout(ResponseTimeoutEvent *timeout)
{
	//TODO: A retry mechanism should be added here to improve the success rate under heavy churn.
	//TODO: Multiple requests to multiple nodes can be sent to improve reliability.
	bool found = false;
	std::vector<ResponseTimeoutEvent *>::iterator timeout_it;
	PeerData peerData;

	//Locate the timeout in the pending requests list
	PendingRequests::iterator it = pendingRequests.find(timeout->getRpcid());

	// a failure response to the higher layer for the received timeout
	sendUpperResponse(it->second.responseType, timeout->getRpcid(), false);

	/*if (it->second.numGetSent > 0)
		std::cout << "[" << simTime() << ":" << this_address <<"]: GET timeout received for peer (" << timeout->getPeerData().getAddress() << " with rpcid " << timeout->getRpcid() << endl;
	else if (it->second.numPutSent > 0)
		std::cout << "[" << simTime() << ":" << this_address <<"]: PUT timeout received for peer (" << timeout->getPeerData().getAddress() << "with rpcid " << timeout->getRpcid() << endl;
	else error("Unknown timeout type.");*/

	//Locate and delete the timeout in the timeout vector in the pending requests list
	//(This is a small list, no larger than the number of required replicas and gets only have one item)
	for (timeout_it = it->second.timeouts.begin() ; timeout_it != it->second.timeouts.end() ; timeout_it++)
	{
		//timeout_it is an iterator to a pointer, so it has to be dereferenced once to get to the ResponseTimeoutEvent pointer
		peerData = (*timeout_it)->getPeerData();

		if (peerData == timeout->getPeerData())
		{
			delete(timeout);
			it->second.timeouts.erase(timeout_it);
			found = true;
			break;
		}
	}

	if (!found)
		error("When a timeout expired, its linked peer data could not be located.");

	//If there are no more timeouts outstanding, remove the pending request item from the requests vector
	if (it->second.timeouts.size() == 0)
		pendingRequests.erase(it);

	//The peer is not removed from the group ledger here. Since the peer itself is contained in its own group ledger, a message is just sent to itself to remove the peer.

	peerLeftInform(peerData);
}

void GroupStorage::handleMessage(cMessage *msg)
{
	ObjectTTLTimer *ttlTimer = NULL;

	if (msg == event)
	{
		//For the first join request, a request is sent to the well known directory server
		TransportAddress destAdr(IPAddress(directory_ip), directory_port);

		joinRequest(destAdr);

		scheduleAt(simTime()+1, event);		//TODO: make the 1 second wait time a configuration variable that may be set
	}
	else if (strcmp(msg->getName(), "timeout") == 0)
	{
		ResponseTimeoutEvent *timeout = check_and_cast<ResponseTimeoutEvent *>(msg);

		handleTimeout(timeout);

	}
	else if ((ttlTimer = dynamic_cast<ObjectTTLTimer*>(msg)) != NULL)
    {
		//If the object's TTL has expired, remove the object from the ledger.
		storage_map.erase(ttlTimer->getKey());
        delete msg;

    }
	else if (strcmp(msg->getArrivalGate()->getName(), "from_upperTier") == 0)
	{
		PositionUpdatePkt *update_pkt = check_and_cast<PositionUpdatePkt *>(msg);

		latitude = update_pkt->getLatitude();
		longitude = update_pkt->getLongitude();

		//A request is sent to the well known directory server
		joinRequest(TransportAddress(IPAddress(directory_ip), directory_port));

		scheduleAt(simTime()+1, event);		//TODO: make the 1 second wait time a configuration variable that may be set

		//Obtain this node's transport address from the communicator module
		const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
		this_address = TransportAddress(thisNode->getIp(), thisNode->getPort());

		delete(msg);
	}
	else {
		Packet *packet = check_and_cast<Packet *>(msg);

		handlePacket(packet);
	}
}
