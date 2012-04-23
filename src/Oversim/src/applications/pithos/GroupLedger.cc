//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "GroupLedger.h"

Define_Module(GroupLedger);

GroupLedger::GroupLedger()
{
	periodicTimer = NULL;
}

GroupLedger::~GroupLedger()
{
	cancelAndDelete(periodicTimer);

	object_map.clear();
	peer_list.clear();
}

void GroupLedger::initialize()
{
	globalStatistics = GlobalStatisticsAccess().get();

	numPeerRemoveFail = 0;
	numPeerRemoveSuccess = 0;
	numObjectGetFail = 0;
	numObjectGetSuccess = 0;
	numPeerKnownError = 0;
	numPeerKnownSuccess = 0;
	numObjectRemoveFail = 0;
	numObjectRemoveSuccess = 0;

	data_size = 0;
	objects_total = 0;
	objects_starved = 0;

	object_lifetime = 0.0;

	periodicTimer = new cMessage("PithosTestMapTimer");

	scheduleAt(simTime(), periodicTimer);
}

void GroupLedger::recordAndClear()
{
	//Every time the group ledger is clear, the statistics are recorded for the time leading up to the clear.
	finish();

	numPeerRemoveFail = 0;
	numPeerRemoveSuccess = 0;
	numObjectGetFail = 0;
	numObjectGetSuccess = 0;
	numPeerKnownError = 0;
	numPeerKnownSuccess = 0;
	numObjectRemoveFail = 0;
	numObjectRemoveSuccess = 0;

	data_size = 0;
	objects_total = 0;
	objects_starved = 0;

	object_lifetime = 0;

	object_map.clear();
	peer_list.clear();
}

/**
 * For validation only.
 * This function was used to verify the statistics collected throughout the simulation of the total number of objects in a group.
 * Continuous collection is preferred above this function, because data is then collected when it's being processed and therefore available by default.
 */
int GroupLedger::countTotalObjects()
{
	int count = 0;

	for (unsigned int i = 0 ; i < peer_list.size() ; i++)
	{
		count += peer_list.at(i).getObjectListSize();
	}

	return count;
}

void GroupLedger::handleMessage(cMessage* msg)
{
    ObjectTTLTimer *ttlTimer = NULL;

    if (msg == periodicTimer)
    {
    	scheduleAt(simTime() + TEST_MAP_INTERVAL, msg);

    	//If the peer is a node within a group, show its super peer address and its own address
		cModule *groupStorageModule = getParentModule()->getSubmodule("group_storage");
		GroupStorage *groupStorage = check_and_cast<GroupStorage *>(groupStorageModule);

		std::ostringstream group_name;

		if (isSuperPeerLedger())
		{
			//If a peer is the super peer of a group, show its own address as both super peer address and peer address
			const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
			TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

			group_name << "GroupLedger (" << thisAdr << ":" << thisAdr <<"): ";

			RECORD_STATS(globalStatistics->recordOutVector((group_name.str() + std::string("Data stored for known group objects")).c_str(), data_size));
			RECORD_STATS(globalStatistics->recordOutVector((group_name.str() + std::string("Number of total group objects")).c_str(), objects_total));
			RECORD_STATS(globalStatistics->recordOutVector((group_name.str() + std::string("Number of starved group objects")).c_str(), objects_starved));

			if (object_map.size() > 0)
				RECORD_STATS(globalStatistics->recordOutVector((group_name.str() + std::string("Average number of replicas per object")).c_str(), double(objects_total)/(object_map.size() + objects_starved)));
			else RECORD_STATS(globalStatistics->recordOutVector((group_name.str() + std::string("Average number of replicas per object")).c_str(), 0));
		}
		else if (!(groupStorage->getSuperPeerAddress().isUnspecified()))
		{
			//std::cout << "Peer address in group is: " << groupStorage->getSuperPeerAddress() << endl;
			const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
			TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

			group_name << "GroupLedger (" << groupStorage->getSuperPeerAddress() << ":" << thisAdr <<"): ";
		}
		else {
			//std::cout << "Unspecified address is: " << groupStorage->getSuperPeerAddress() << endl;
			return;
		}

		//Collect stats for both super peers and peers
		RECORD_STATS(globalStatistics->recordOutVector((group_name.str() + std::string("Number of known group peers")).c_str(), peer_list.size()));
		RECORD_STATS(globalStatistics->recordOutVector((group_name.str() + std::string("Number of unique group objects")).c_str(), object_map.size()));
    }
    else if ((ttlTimer = dynamic_cast<ObjectTTLTimer*>(msg)) != NULL)
	{
		//If the object's TTL has expired, remove the object from the ledger.
		removeObject(ttlTimer->getKey());
		delete msg;
	}
    else {
        throw cRuntimeError("[GroupLedger::handleMessage()]: Unknown message type!");
    }
}

ObjectData GroupLedger::getObjectFromPeer(PeerData peer_data, const int &i)
{
	PeerLedgerList::iterator peer_ledger_it;

	for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
	{
		if (*(peer_ledger_it->peerDataPtr) == peer_data)
		{
			return *(peer_ledger_it->getObjectRef(i));
		}
	}

	error("Peer not found in group ledger,");
	return ObjectData();
}

int GroupLedger::getObjectLedgerSize(PeerData peer_data)
{
	PeerLedgerList::iterator peer_ledger_it;

	//std::cout << "\nPeer data to match for size: " << peer_data.getAddress() << endl;

	for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
	{
		//std::cout << "Currently comparing peer: " << peer_ledger_it->peerDataPtr->getAddress() << endl;

		if (*(peer_ledger_it->peerDataPtr) == peer_data)
			return peer_ledger_it->getObjectListSize();
	}

	return -1;
}

int GroupLedger::getPeerLedgerSize(ObjectData object_data)
{
	ObjectLedgerMap::iterator object_map_it = object_map.find(object_data.getKey());
	if (object_map_it == object_map.end())
		error("[getPeerLedgerSize]: Object could not be found in group.");

	return object_map_it->second.getPeerListSize();
}

bool GroupLedger::isSuperPeerLedger()
{
	if (strcmp(getName(), "sp_group_ledger") == 0)
		return true;
	else if  (strcmp(getName(), "group_ledger") == 0)
		return false;
	else


	error("Group ledger named incorrectly, expecting 'group_ledger' or 'sp_group_ledger'.");
	return false;	//This value will never be returned.
}

void GroupLedger::finish()
{
	//Bear in mind that whenever recordAndClear() is called, finish() is also called.

    // record scalar data
	cModule *communicatorModule = getParentModule()->getSubmodule("communicator");
	Communicator *communicator = check_and_cast<Communicator *>(communicatorModule);

    cModule *groupStorageModule = getParentModule()->getSubmodule("group_storage");
    GroupStorage *groupStorage = check_and_cast<GroupStorage *>(groupStorageModule);

    simtime_t time = globalStatistics->calcMeasuredLifetime(communicator->getCreationTime());

    if ((time >= GlobalStatistics::MIN_MEASURED) && !(groupStorage->getSuperPeerAddress().isUnspecified()))
    {
    	std::ostringstream group_name;

    	if (isSuperPeerLedger())
    	{
    		const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
    		TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

    		group_name << "GroupLedger (" << thisAdr << "): ";
    	} else {

        	group_name << "GroupLedger (" << groupStorage->getSuperPeerAddress() << "): ";
    	}

    	globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Failed object gets")).c_str() , numObjectGetFail);
    	globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Successful object gets")).c_str() , numObjectGetSuccess);
		globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Failed peer gets")).c_str(), numPeerRemoveFail);
		globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Successful peer gets")).c_str(), numPeerRemoveSuccess);
		globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Number of known peer insertion attempts")).c_str(), numPeerKnownError);
		globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Number of unknown peer insertions")).c_str(), numPeerKnownSuccess);
		globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Failed object removals")).c_str() , numObjectRemoveFail);
		globalStatistics->addStdDev((group_name.str() + std::string("GroupLedger: Successful object removals")).c_str() , numObjectRemoveSuccess);
    }
}

bool GroupLedger::isObjectInGroup(OverlayKey key)
{
	return object_map.find(key) != object_map.end();
}

bool GroupLedger::isPeerInGroup(PeerData peerData)
{
	PeerLedgerList::iterator peer_ledger_it;

	for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
	{
		if (*(peer_ledger_it->peerDataPtr) == peerData)
			return true;
	}

	return false;
}

//****Careful, this is a very expensive function and should only be used for debugging purposes****
bool GroupLedger::verifyObjectOnPeer(ObjectData object_data, PeerData peer_data)
{
	unsigned int i;
	ObjectLedgerMap::iterator object_map_it;
	ObjectLedger object_ledger_entry;
	PeerDataPtr peer_ptr;

	PeerLedger peer_ledger_entry;
	ObjectDataPtr object_ptr;
	bool found = true;

	//Firstly check whether the peer is listed in the object's peer list.
	object_map_it = object_map.find(object_data.getKey());
	if (object_map_it == object_map.end())
		error("Object could not be found in group.");

	object_ledger_entry = object_map_it->second;

	for (i = 0; i < object_ledger_entry.getPeerListSize() ; i++)
	{
		peer_ptr = object_ledger_entry.getPeerRef(i);

		if (*peer_ptr == peer_data)
			break;
	}

	if (i == object_ledger_entry.getPeerListSize())
		found = false;

	//Secondly, check whether the object is listed in the peer's object map
	for (i = 0 ; i < peer_list.size() ; i++)
	{
		peer_ledger_entry = peer_list.at(i);

		if (*(peer_ledger_entry.peerDataPtr) == peer_data)
			break;
	}

	if (i == peer_list.size())
		error("Peer data not found in peer list.");

	for (i = 0; i < peer_ledger_entry.getObjectListSize() ; i++)
	{
		object_ptr = peer_ledger_entry.getObjectRef(i);

		if (*object_ptr == object_data)
			break;
	}

	if (i == peer_ledger_entry.getObjectListSize())
			found = false;

	return found;
}

bool GroupLedger::isObjectOnPeer(ObjectData object_data, PeerData peer_data)
{
	ObjectLedgerMap::iterator object_map_it = object_map.find(object_data.getKey());
	if (object_map_it == object_map.end())
		return false;
		//error("[isObjectOnPeer]: Object could not be found in group.");

	return object_map_it->second.isPeerPresent(peer_data);
}

int GroupLedger::getReplicaNum(ObjectData object_data)
{
	ObjectLedgerMap::iterator object_map_it = object_map.find(object_data.getKey());
	if (object_map_it == object_map.end())
		error("[getReplicaNum]: Object could not be found in group.");

	return object_map_it->second.getPeerListSize();
}

PeerData GroupLedger::getRandomPeer(OverlayKey key)
{
	ObjectLedgerMap::iterator object_map_it;
	ObjectLedger object_ledger_entry;
	PeerDataPtr peer_ptr;

	object_map_it = object_map.find(key);
	if (object_map_it == object_map.end())
		error("Object could not be found in group.");

	object_ledger_entry = object_map_it->second;

	peer_ptr = object_ledger_entry.getRandPeerRef();

	return *peer_ptr;
}

PeerData GroupLedger::getRandomPeer()
{
    if (peer_list.size() == 0)
        error("No peers in group.");

    return *((peer_list.at(intuniform(0, peer_list.size()-1))).peerDataPtr);
}

void GroupLedger::addPeer(PeerData peer_dat)
{
	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

	//std::cout << "Adding peer with address: " << peer_dat.getAddress() << endl;
	//If the peer is not known, add it to the peer list
	if (!isPeerInGroup(peer_dat))
	{
		PeerLedger peer_ledger;
		peer_ledger.peerDataPtr = PeerDataPtr(new PeerData(peer_dat));
		peer_list.push_back(peer_ledger);

		/*if (isSuperPeerLedger())
			std::cout << "[" << simTime() << ":super peer " << thisAdr << "]: Added peer from super peer: " << peer_dat.getAddress() << endl;
		else std::cout << "[" << simTime() << ":peer " << thisAdr << "]: Added peer from super peer: " << peer_dat.getAddress() << endl;*/
		RECORD_STATS(numPeerKnownSuccess++);
	} else {

		/*if (isSuperPeerLedger())
			std::cout << "[" << simTime() << ":super peer " << thisAdr << "]: Failed to add peer from super peer (peer already known): " << peer_dat.getAddress() << endl;
		else std::cout << "[" << simTime() << ":peer " << thisAdr << "]: Failed to add peer from super peer (peer already known): " << peer_dat.getAddress() << endl;*/
		RECORD_STATS(numPeerKnownError++);
	}
}

void GroupLedger::removePeer(PeerData peer_dat)
{
	PeerLedgerList::iterator peer_ledger_it;
	ObjectDataPtr object_data_ptr;
	ObjectLedgerMap::iterator object_ledger_it;
	unsigned int objectListSize;

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

	/*if (isSuperPeerLedger())
		std::cout << "[" << simTime() << ":super peer " << thisAdr << "]: Peer slated for removal: " << peer_dat.getAddress() << endl;
	else std::cout << "[" << simTime() << ":peer " << thisAdr << "]: Peer slated for removal: " << peer_dat.getAddress() << endl;*/

	//Find the peer in the peer list
	for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
	{
		if (*(peer_ledger_it->peerDataPtr) == peer_dat)
		{
			break;
		}
	}

	std::ostringstream msg;
	msg << "[" << thisAdr << "]: Peer remove error\n";

	if (peer_ledger_it == peer_list.end())
	{
		//error("Peer slated for removal not found in group.");
		RECORD_STATS(numPeerRemoveFail++);
		RECORD_STATS(globalStatistics->recordOutVector(msg.str().c_str(), 10));
		return;
	} else {
		RECORD_STATS(numPeerRemoveSuccess++);
		RECORD_STATS(globalStatistics->recordOutVector(msg.str().c_str(), 0));
	}

	objectListSize = peer_ledger_it->getObjectListSize();

	//Iterate through all object references listed for the peer
	for (unsigned int i = 0 ; i < objectListSize ; i++)
	{
		object_data_ptr = peer_ledger_it->getObjectRef(i);

		//Retrieve the object ledger for the listed object reference
		object_ledger_it = object_map.find(object_data_ptr->getKey());
		if (object_ledger_it == object_map.end())
		{
			RECORD_STATS(numObjectGetFail++);
			error("Object not found when removing peer.");
		} else RECORD_STATS(numObjectGetSuccess++);

		//Remove the specific peer reference from the object ledger
		object_ledger_it->second.erasePeerRef(peer_ledger_it->peerDataPtr);

		data_size -= object_ledger_it->second.objectDataPtr->getSize();
		objects_total--;

		//If the peer is removed and there are now no peers on which the object is stored, remove the object ledger entry
		if (object_ledger_it->second.getPeerListSize() == 0)
		{
			std::ostringstream group_name;

			if (isSuperPeerLedger())
			{
				object_lifetime = (simTime() - object_ledger_it->second.objectDataPtr->getCreationTime()).dbl();
				std::cout << "Object lifetime: " << object_lifetime << endl;
				RECORD_STATS(globalStatistics->recordOutVector("GroupLedger: Object lifetime", object_lifetime));
				RECORD_STATS(globalStatistics->recordOutVector("GroupLedger: Object initial group size", object_ledger_it->second.objectDataPtr->getInitGroupSize()));
			}

			object_map.erase(object_ledger_it);
			objects_starved++;
		}
	}

	peer_list.erase(peer_ledger_it);
}

void GroupLedger::removeObject(OverlayKey key)
{
	ObjectLedgerMap::iterator object_ledger_it;
	PeerDataPtr peer_data_ptr;
	PeerLedgerList::iterator peer_ledger_it;

	object_ledger_it = object_map.find(key);
	if (object_ledger_it == object_map.end())
	{
		RECORD_STATS(numObjectRemoveFail++);
		return;
	} else { RECORD_STATS(numObjectRemoveSuccess++); }

	//Iterate through all peer references listed for the object
	for (unsigned int i = 0 ; i < object_ledger_it->second.getPeerListSize() ; i++)
	{
		peer_data_ptr = object_ledger_it->second.getPeerRef(i);

		//Find the peer in the peer list
		for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
		{
			if (peer_ledger_it->peerDataPtr == peer_data_ptr)
			{
				break;
			}
		}

		if (peer_ledger_it == peer_list.end())
			error("Peer not found when removing object.");

		//Remove the specific peer reference from the object ledger
		peer_ledger_it->eraseObjectRef(object_ledger_it->second.objectDataPtr);

		//Records the total size in object bytes that's stored in this ledger.
		data_size -= object_ledger_it->second.objectDataPtr->getSize();
		objects_total--;

		//Peers do not have to house objects to exist. A peer can exist, even if it stores no objects.
	}

	//TODO: Uncommenting this says that objects may exist, without being stored on any peer. This helps to tracks objects that have starved.
	object_map.erase(object_ledger_it);
}

void GroupLedger::addObject(ObjectData objectData, PeerData peer_data_recv)
{
	PeerLedgerList::iterator peer_ledger_it;
	ObjectLedgerMap::iterator object_map_it;
	ObjectLedger *object_ledger;
	std::pair<ObjectLedgerMap::iterator,bool> ret;

	Enter_Method_Silent();

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress thisAdr(thisNode->getIp(), thisNode->getPort());

	//If the object has expired since it was sent, do not add it to the ledger.
	if (objectData.getCreationTime() + objectData.getTTL() < simTime())
		return;

	//Check whether the received object information is already stored in the super peer
	object_map_it = object_map.find(objectData.getKey());

	//If the object is not already known, have the iterator point to a new object instead.
	if (object_map_it == object_map.end())
	{
		//TODO: The fact that a duplicate key was received should be logged
		object_ledger = new ObjectLedger();

		//Log the file name and what peers it is stored on
		object_ledger->objectDataPtr.reset(new ObjectData(objectData));

	} else object_ledger = &(object_map_it->second);

	for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
	{
		if (*(peer_ledger_it->peerDataPtr) == peer_data_recv)
			break;
	}

	//If an object is stored on an unknown peer, first add that peer to the peer list
	if (peer_ledger_it == peer_list.end())
	{
		//TODO: Log this exception
		PeerLedger peer_ledger;
		peer_ledger.peerDataPtr.reset(new PeerData(peer_data_recv));

		//std::cout << "[" << thisAdr << "]: ";
		peer_ledger.addObjectRef(object_ledger->objectDataPtr);
		peer_list.push_back(peer_ledger);

		/*if (isSuperPeerLedger())
			std::cout << "[" << simTime() << ":super peer" << thisAdr << "]: Added peer because of unknown object: " << peer_ledger.peerDataPtr->getAddress() << endl;
		else std::cout << "[" << simTime() << ":peer " << thisAdr << "]: Added peer because of unknown object: " << peer_ledger.peerDataPtr->getAddress() << endl;*/

		object_ledger->addPeerRef(peer_ledger.peerDataPtr);	//Add a peer to the ObjectInfo object's peer vector

	} else {

		//std::cout << "[" << thisAdr << "]: ";
		peer_ledger_it->addObjectRef(object_ledger->objectDataPtr);
		object_ledger->addPeerRef(peer_ledger_it->peerDataPtr);	//Add a peer to the ObjectInfo object's peer vector
	}

	if (object_map_it == object_map.end())
	{
		ret = object_map.insert(std::make_pair(objectData.getKey(), *object_ledger));
		//Ensure that a duplicate key wasn't inserted
		if (ret.second == false)
			error("[GroupLedger::addObject]: Duplicate key inserted into storage.");

		delete(object_ledger);
	}

	data_size += objectData.getSize();
	objects_total++;

	//Schedule the object to be removed when its TTL expires.
	ObjectTTLTimer* timer = new ObjectTTLTimer();
	timer->setKey(objectData.getKey());
	scheduleAt(objectData.getCreationTime() + objectData.getTTL(), timer);
}

unsigned int GroupLedger::getGroupSize()
{
	return peer_list.size();
}

unsigned int GroupLedger::getNumGroupObjects()
{
	return object_map.size();
}

PeerDataPtr GroupLedger::getPeerPtr(const int &i)
{
	return (peer_list.at(i)).peerDataPtr;
}

ObjectLedgerMap::iterator GroupLedger::getObjectMapBegin()
{
	return object_map.begin();
}

ObjectLedgerMap::iterator GroupLedger::getObjectMapEnd()
{
	return object_map.end();
}
