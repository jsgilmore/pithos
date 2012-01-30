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

GroupStorage::~GroupStorage()
{
	PendingRequests::iterator requests_it;
	std::vector<ResponseTimeoutEvent *>::iterator timeout_it;


	for (requests_it = pendingRequests.begin(); requests_it != pendingRequests.end(); requests_it++)
	{
		for (timeout_it = requests_it->second.timeouts.begin() ; timeout_it != requests_it->second.timeouts.end() ; timeout_it++)
		{
			cancelAndDelete(*timeout_it);
		}
		requests_it->second.timeouts.clear();
	}

	pendingRequests.clear();
}

void GroupStorage::initialize()
{
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

	//Read the IP address and port of the directory server from the NED file.
	strcpy(directory_ip, par("directory_ip"));
	directory_port = par("directory_port");

	//Set the event that will initiate the group join procedure.
	event = new cMessage("event");
	scheduleAt(simTime()+par("wait_time"), event);

	double requestTimeout_ms = par("requestTimeout");

	std::cout << "The set timeout is: " << requestTimeout_ms << endl;

	requestTimeout = requestTimeout_ms/1000;	//Devide by a thousands to convert from seconds to milliseconds

	//Assign a random location to the peer in the virtual world
	latitude = uniform(0,100);		//Make this range changeable
	longitude = uniform(0,100);		//Make this range changeable

	globalStatistics = GlobalStatisticsAccess().get();

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
}

void GroupStorage::finish()
{
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

    simtime_t time = globalStatistics->calcMeasuredLifetime(communicator->getCreationTime());

    if (time >= GlobalStatistics::MIN_MEASURED) {
        // record scalar data
        globalStatistics->addStdDev("GroupStorage: Sent Total Messages/s", numSent / time);

        globalStatistics->addStdDev("GroupStorage: Sent PUT Messages/s", numPutSent / time);
        globalStatistics->addStdDev("GroupStorage: Failed PUT Requests/s", numPutError / time);
        globalStatistics->addStdDev("GroupStorage: Successful PUT Requests/s", numPutSuccess / time);

        globalStatistics->addStdDev("GroupStorage: Sent GET Messages/s", numGetSent / time);
		globalStatistics->addStdDev("GroupStorage: Failed GET Requests/s", numGetError / time);
		globalStatistics->addStdDev("GroupStorage: Successful GET Requests/s", numGetSuccess / time);

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

		//SourceAddress + DestinationAddress + ResponseType + PayloadType + isSuccess + RPCID
		(*response)->setByteLength(4 + 4 + 4 + 4 + 4 + 4 + object.getSize());
	} else {
		//SourceAddress + DestinationAddress + ResponseType + PayloadType + isSuccess + RPCID
		(*response)->setByteLength(4 + 4 + 4 + 4 + 4 + 4);
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

//TODO: Figure out how group storage will do responses.
void GroupStorage::sendUpperResponse(int responseType, unsigned int rpcid, bool isSuccess, GameObject object)
{
	ResponsePkt *response;

	createResponseMsg(&response, responseType, rpcid, isSuccess, object);

	send(response, "read");
}

//This is kind of a recursive distributed network function. Keep that in mind when figuring out the code+comments
void GroupStorage::requestRetrieve(OverlayKeyPkt *retrieve_req)
{
	ObjectInfo object_info;
	int peer_list_size;
	PeerDataPtr container_peer_ptr;
	ObjectInfoMap::iterator object_map_it;

	OverlayKey *key = &(retrieve_req->getKey());
	unsigned int rpcid = retrieve_req->getValue();		//TODO: Make sure the rpcid is required as a separate variable, even if the mesasge is sent out as is.

	StorageMap::iterator storage_map_it = storage_map.find(*key);

	//Check whether this is a group object
	object_map_it = object_map.find(*key);
	if (object_map_it == object_map.end())
	{
		RECORD_STATS(numGetError++);
		//If the object is not stored in the group, send a failure response to the higher layer
		sendUpperResponse(GROUP_GET, rpcid, false);
		delete(retrieve_req);
		return;
	}

	//If the object is stored in the group, check whether the object is on the same peer that sent the request
	//If the source and destination addresses are the same it means the request comes from the higher layer and not another peer
	if ((storage_map_it != storage_map.end()) && (retrieve_req->getSourceAddress() == retrieve_req->getDestinationAddress()))
	{
		RECORD_STATS(numGetSuccess++);
		//If the object is stored in local storage, send it to the upper layer without requesting from the group
		sendUpperResponse(GROUP_GET, rpcid, true, storage_map_it->second);
		delete(retrieve_req);
		return;
	}

	//Check whether the object is on this peer, but another peer sent the request.
	if (storage_map_it != storage_map.end())
	{
		sendUDPResponse(retrieve_req->getDestinationAddress(), retrieve_req->getSourceAddress(), GROUP_GET, rpcid, true, storage_map_it->second);
		delete(retrieve_req);
		return;
	}

	RECORD_STATS(numSent++; numGetSent++);

	object_info = object_map_it->second;

	peer_list_size = object_info.getPeerListSize();

	container_peer_ptr = object_info.getPeerRef(intuniform(0, peer_list_size-1));

	//Send a retrieve request to the group peer storing the object
	retrieve_req->setDestinationAddress(container_peer_ptr->getAddress());

	send(retrieve_req, "comms_gate$o");

	ResponseTimeoutEvent *timeout = new ResponseTimeoutEvent("timeout");
	timeout->setRpcid(rpcid);
	timeout->setPeerDataPtr(container_peer_ptr);
	scheduleAt(simTime()+requestTimeout, timeout);

	PendingRequestsEntry entry;
	entry.numGetSent = 1; //TODO:This numSent should be calculated from the required group and overlay writes
	entry.responseType = GROUP_GET;
	entry.timeouts.push_back(timeout);
	pendingRequests.insert(std::make_pair(rpcid, entry));
}

void GroupStorage::addObject(cMessage *msg)
{
	PeerListPkt *plist_p = check_and_cast<PeerListPkt *>(msg);

	std::vector<PeerDataPtr>::iterator peer_it;
	ObjectInfoMap::iterator object_map_it;
	PeerData peer_data_recv;
	ObjectInfo *object_info;

	//Check whether the received object information is already stored in the super peer
	object_map_it = object_map.find(plist_p->getObjectKey());

	//If the object is not already known, have the iterator point to a new object instead.
	if (object_map_it == object_map.end())
	{
		//TODO: The fact that a duplicate key was received should be logged
		object_info = new ObjectInfo();

		//Log the file name and what peers it is stored on
		object_info->setObjectName(plist_p->getObjectName());
		object_info->setSize(plist_p->getObjectSize());

	} else object_info = &(object_map_it->second);

	//Iterate through all PeerData objects received in the PeerListPkt
	for (unsigned int i = 0 ; i < plist_p->getPeer_listArraySize() ; i++)
	{
		peer_data_recv = ((PeerData)plist_p->getPeer_list(i));

		for (peer_it = group_peers.begin() ; peer_it != group_peers.end() ; peer_it++)
		{
			//*peer_it returns a PeerDataPtr type, which again has to be dereferenced to obtain the PeerData object (**peer_it)
			if (**peer_it == peer_data_recv)
				break;
		}

		//If an object is stored on an unknown peer, first add that peer to the peer list
		if (peer_it == group_peers.end())
		{
			//TODO: Log this exception
			PeerDataPtr peer_dat_ptr(new PeerData(peer_data_recv));
			group_peers.push_back(peer_dat_ptr);

			//Make sure this peer is not listed in the object info peer vector
			if (object_info->isPeerPresent(peer_dat_ptr))
				error("This peer is already known to this object");

			object_info->addPeerRef(peer_dat_ptr);	//Add a peer to the ObjectInfo object's peer vector

		} else {

			//Make sure this peer is not listed in the object info peer vector
			if (object_info->isPeerPresent(*peer_it))
				error("This peer is already known to this object");

			object_info->addPeerRef(*peer_it);	//Add a peer to the ObjectInfo object's peer vector
		}
	}

	if (object_map_it == object_map.end())
	{
		object_map.insert(std::make_pair(plist_p->getObjectKey(), *object_info));
		delete(object_info);
	}
}

void GroupStorage::updatePeerObjects(GameObject go)
{
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	PeerListPkt *objectAddPkt = new PeerListPkt();
	objectAddPkt->setByteLength(4+4+4+8+4);	//Source address, dest address, type, object name ID, storage peer address

	if (super_peer_address.isUnspecified())
	{
		//TODO: This error condition should be logged
		EV << "No super peer has been identified. The group object will not be stored on this peer\n";
		delete(objectAddPkt);
		return;
	}

	objectAddPkt->setObjectName(go.getObjectName());
	objectAddPkt->setObjectSize(go.getSize());
	objectAddPkt->setObjectKey(go.getHash());
	objectAddPkt->setPayloadType(OBJECT_ADD);
	objectAddPkt->setSourceAddress(sourceAdr);
	objectAddPkt->setName("object_add");

	objectAddPkt->addToPeerList(PeerData(sourceAdr));

	//Inform all group peers about the new object and where it is stored
	for (unsigned int i = 0 ; i < group_peers.size() ; i++)
	{
		TransportAddress dest_adr = (*(group_peers.at(i))).getAddress();
		objectAddPkt->setDestinationAddress(dest_adr);
		send(objectAddPkt->dup(), "comms_gate$o");		//Set address
	}

	//Inform the super peer of the new object and the peer that contains it
	//The super peer object type must be different to the group object type for communicator routing purposes
	objectAddPkt->setPayloadType(SP_OBJECT_ADD);
	objectAddPkt->setDestinationAddress(super_peer_address);
	send(objectAddPkt, "comms_gate$o");		//Set address
}

PeerDataPtr GroupStorage::selectDestination(std::vector<TransportAddress> send_list)
{
	unsigned int j;
	bool original_address = false;
	PeerDataPtr peerDataPtr;

	while(!original_address)
	{
		peerDataPtr = group_peers.at(intuniform(0, group_peers.size()-1));		//Choose a random peer in the group for the destination

		//Check all previous chosen addresses to determine whether this address is unique
		original_address = true;

		for (j = 0 ; j < send_list.size() ; j++)
		{
			if (send_list.at(j) == peerDataPtr->getAddress())
				original_address = false;
		}
	}

	return peerDataPtr;
}

void GroupStorage::createWritePkt(ValuePkt **write, unsigned int rpcid)
{
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	//Create the packet that will house the game object
	(*write) = new ValuePkt("write");
	(*write)->setByteLength(4+4+4+8);	//Source address, dest address, object name ID and object size
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
		//This packet is sent internally, so no size is required

		//Send one failure response packet for each replica that cannot be stored
		for (i = 0 ; i < replicas - group_peers.size() - 1 ; i++)
			send(response->dup(), "read");
		send(response, "read");

		emit(groupSendFailSignal, replicas - group_peers.size());
		RECORD_STATS(numPutError++);
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
	std::vector<TransportAddress> send_list;

	PendingRequestsEntry entry;
	ResponseTimeoutEvent *timeout;
	PeerDataPtr destAdrPtr;

	replicas = getReplicaNr(rpcid);

	createWritePkt(&write, rpcid);

	//Add a new request for which at least one response is required
	//std::cout << "Inserting pending put request with rpcid: " << rpcid << endl;
	entry.numPutSent = replicas;
	entry.responseType = GROUP_PUT;

	for (i = 0 ; i < replicas ; i++)
	{
		//Duplicates the objects for sending
		go_dup = go->dup();
		write_dup = write->dup();

		write_dup->addObject(go_dup);

		destAdrPtr = selectDestination(send_list);
		write_dup->setDestinationAddress(destAdrPtr->getAddress());

		send_list.push_back(destAdrPtr->getAddress());

		RECORD_STATS(numSent++; numPutSent++);
		send(write_dup, "comms_gate$o");

		timeout = new ResponseTimeoutEvent("timeout");
		timeout->setRpcid(rpcid);
		timeout->setPeerDataPtr(destAdrPtr);
		scheduleAt(simTime()+requestTimeout, timeout);

		entry.timeouts.push_back(timeout);
	}

	pendingRequests.insert(std::make_pair(rpcid, entry));

	delete(write);
}

void GroupStorage::respond_toUpper(cMessage *msg)
{
	bool found = false;
	ResponsePkt *response = check_and_cast<ResponsePkt *>(msg);

	std::vector<ResponseTimeoutEvent *>::iterator timeout_it;
	PendingRequests::iterator it = pendingRequests.find(response->getRpcid());

	if (it != pendingRequests.end()) // unknown request or request for already erased call
	{
		//Cancel the timeout for the responding peer
		for (timeout_it = it->second.timeouts.begin() ; timeout_it != it->second.timeouts.end() ; timeout_it++)
		{
			ResponseTimeoutEvent * timeout = *timeout_it;
			PeerDataPtr peerDataPtr = timeout->getPeerDataPtr();
			TransportAddress address = peerDataPtr->getAddress();


			//timeout_it is an iterator to a pointer, so it has to be dereferenced once to get to the ResponseTimeoutEvent pointer
			if (address == response->getSourceAddress())
			{
				cancelAndDelete(timeout);
				it->second.timeouts.erase(timeout_it);
				found = true;
				break;
			}
		}

		if (!found)
			error("No timeout found for response message from peer.");

		//TODO: Record the group put latency (This will merely require that the response packet be expanded with the initiation time of the request)

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
	} else error("Unknown response received.");

	send(msg, "read");
}

int GroupStorage::getStorageBytes()
{
	int total_size = 0;

	StorageMap::iterator it;

	//This is inefficient, since a sequential search will be done for every element in the queue.
	//TODO: The "forEachChild" method should rather be implemented with an appropriate visitor class.
	for (it = storage_map.begin() ; it != storage_map.end() ; it++)
	{
		total_size += it->second.getSize();
	}

	return total_size;
}

void GroupStorage::store(cMessage *msg)
{
	simtime_t delay;
	ValuePkt *value_pkt = check_and_cast<ValuePkt *>(msg);

	if (!(msg->hasObject("GameObject")))
		error("Storage received a message with no game object attached");

	GameObject *go = (GameObject *)msg->getObject("GameObject");		//We're not removing the object here, because we're only using the value and then the object is deleted with the message.

	go->setGroupAddress(super_peer_address);

	EV << getName() << " " << getIndex() << " received Game Object of size " << go->getSize() << "\n";

	delay = simTime() - go->getCreationTime();

	EV << getName() << " " << getIndex() << " received write command of size " << go->getSize() << " with delay " << go->getCreationTime() << "\n";

	//std::cout << "[GroupStorageTarget] Stored object with key " << go->getHash() << endl;

	storage_map.insert(std::make_pair(go->getHash(), *go));

	emit(storeTimeSignal, delay);

	emit(qlenSignal, storage_map.size());
	emit(qsizeSignal, getStorageBytes());

	emit(objectsSignal, 1);

	sendUDPResponse(value_pkt->getDestinationAddress(), value_pkt->getSourceAddress(), GROUP_PUT, value_pkt->getValue(), true);

	updatePeerObjects(*go);
}

void GroupStorage::addPeers(cMessage *msg)
{
	PeerListPkt *list_p = check_and_cast<PeerListPkt *>(msg);
	PeerData peer_dat;
	PeerDataPtr peer_dat_ptr;
	unsigned int i;
	bool found;
	simtime_t joinTime = simTime();

	//If we didn't know of any peers in our group and we've now been informed of some, inform the game module to start producing requests
	//This is also the time when we record that we've successfully joined a group (When we've joined a super peer and we know of other peers in the group).
	if ((group_peers.size() == 0) && (list_p->getPeer_listArraySize() > 0))
	{
		AddressPkt *request_start = new AddressPkt("request_start");
		request_start->setAddress(super_peer_address);
		request_start->setByteLength(4);	//The super peer IP
		send(request_start, "to_upperTier");

		emit(joinTimeSignal, joinTime);
	}

	for ( i = 0 ; i < list_p->getPeer_listArraySize() ; i++)
	{
		peer_dat = list_p->getPeer_list(i);
		found = false;

		//First check whether this peer is already known
		for (unsigned int j = 0 ; j < group_peers.size() ; j++)
		{
			if (*(group_peers.at(j)) == peer_dat)
			{
				found = true;
				break;
			}
		}

		//If the peer is not known, add it to the peer list
		if (!found)
		{
			peer_dat_ptr.reset(new PeerData(peer_dat));
			group_peers.push_back(peer_dat_ptr);
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

void GroupStorage::handlePacket(Packet *packet)
{

	if (packet->getPayloadType() == INFORM)
	{
		//Data was received from the UDP layer by the communicator and has been referred to the Peer logic
		addAndJoinSuperPeer(packet);
		delete(packet);
	} else if (packet->getPayloadType() == JOIN_ACCEPT)
	{
		addPeers(packet);
		delete(packet);
	} else if (packet->getPayloadType() == WRITE)
	{
		store(packet);
		delete(packet);
	} else if (packet->getPayloadType() == RESPONSE)
	{
		respond_toUpper(packet);
	} else if (packet->getPayloadType() == STORE_REQ)
	{
		ValuePkt *store_req = check_and_cast<ValuePkt *>(packet);

		GameObject *go = (GameObject *)packet->removeObject("GameObject");
		if (go == NULL)
			error("No object was attached to be stored in group storage");

		send_forstore(go, store_req->getValue());

		delete(go);		//Only duplicates of the game object are stored, so the original must be deleted
		delete(packet);
	} else if (packet->getPayloadType() == RETRIEVE_REQ)
	{
		OverlayKeyPkt *retrieve_req = check_and_cast<OverlayKeyPkt *>(packet);

		requestRetrieve(retrieve_req);
	} else if (packet->getPayloadType() == OBJECT_ADD)
	{
		addObject(packet);
		delete(packet);
	}
	else error("Group storage received an unknown packet");
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

	}
	else if (strcmp(msg->getName(), "timeout") == 0)
	{
		//If a response has timed out.
		ResponseTimeoutEvent *timeout = check_and_cast<ResponseTimeoutEvent *>(msg);

		//std::cout << "Timeout received for RPCID " << timeout->getRpcid() << endl;

		PendingRequests::iterator it = pendingRequests.find(timeout->getRpcid());

		//Inform the higher layer that the group request has failed
		//TODO: A retry mechanism should be added here to improve the success rate under heavy churn.
		//TODO: Multiple requests to multiple nodes can be sent to improve reliability.
		if (it->second.responseType == GROUP_PUT)
		{
			unsigned int replicas = par("replicas");
			//The higher layer expects a report of every sent put request and replica request
			for (unsigned int i = 0 ; i < replicas ; i++)
			{
				sendUpperResponse(it->second.responseType, timeout->getRpcid(), false);
			}
		} else sendUpperResponse(it->second.responseType, timeout->getRpcid(), false);

		delete(msg);
		pendingRequests.erase(it);
	} else {
		Packet *packet = check_and_cast<Packet *>(msg);

		handlePacket(packet);
	}
}
