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

GroupLedger::GroupLedger()
{
	// TODO Auto-generated constructor stub

}

GroupLedger::~GroupLedger()
{
	object_map.clear();
	peer_list.clear();
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

PeerDataPtr GroupLedger::getRandomPeer(OverlayKey key)
{
	ObjectLedgerMap::iterator object_map_it;
	ObjectLedger object_ledger_entry;
	PeerDataPtr peer_ptr;

	object_map_it = object_map.find(key);
	if (object_map_it == object_map.end())
		opp_error("Object could not be found in group.");

	object_ledger_entry = object_map_it->second;

	peer_ptr = object_ledger_entry.getPeerRef(intuniform(0, object_ledger_entry.getPeerListSize()-1));

	return peer_ptr;
}

PeerDataPtr GroupLedger::getRandomPeer()
{
    if (peer_list.size() == 0)
        opp_error("No peers in group.");

    return (peer_list.at(intuniform(0, peer_list.size()-1))).peerDataPtr;
}

void GroupLedger::addPeer(PeerData peer_dat)
{
	//std::cout << "Adding peer with address: " << peer_dat.getAddress() << endl;
	//If the peer is not known, add it to the peer list
	if (!isPeerInGroup(peer_dat))
	{
		PeerLedger peer_ledger;
		peer_ledger.peerDataPtr = PeerDataPtr(new PeerData(peer_dat));
		peer_list.push_back(peer_ledger);
	}// else opp_error("Peer is already in group.");
}

void GroupLedger::removePeer(PeerData peer_dat)
{
	PeerLedgerList::iterator peer_ledger_it;
	ObjectDataPtr object_data_ptr;
	ObjectLedgerMap::iterator object_ledger_it;

	//std::cout << "Peer slated for removal: " << peer_dat.getAddress() << endl;

	//Find the peer in the peer list
	for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
	{
		if (*(peer_ledger_it->peerDataPtr) == peer_dat)
		{
			break;
		}
	}

	if (peer_ledger_it == peer_list.end())
	{
		//opp_error("Peer slated for removal not found in group.");
		return;
	}

	//Iterate through all object references listed for the peer
	for (unsigned int i = 0 ; i < peer_ledger_it->getObjectListSize() ; i++)
	{
		object_data_ptr = peer_ledger_it->getObjectRef(i);

		//Retrieve the object ledger for the listed object reference
		object_ledger_it = object_map.find(object_data_ptr->getKey());

		//Remove the specific peer reference from the object ledger
		object_ledger_it->second.erasePeerRef(peer_ledger_it->peerDataPtr);
		//If the peer is removed and there are now no peers on which the object is stored, remove the object ledger entry
		if (object_ledger_it->second.getPeerListSize() == 0)
			object_map.erase(object_ledger_it);
	}

	peer_list.erase(peer_ledger_it);
}

void GroupLedger::removeObject(ObjectData object_data)
{
	ObjectLedgerMap::iterator object_ledger_it;
	PeerDataPtr peer_data_ptr;
	PeerLedgerList::iterator peer_ledger_it;

	object_ledger_it = object_map.find(object_data.getKey());
	if (object_ledger_it == object_map.end())
			opp_error("Object slated for removal not found in group.");

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

		//Remove the specific peer reference from the object ledger
		peer_ledger_it->eraseObjectRef(object_ledger_it->second.objectDataPtr);

		//Peers do not have to house objects to exist. A peer can exist, even if it stores no objects.
	}

	object_map.erase(object_ledger_it);
}

void GroupLedger::addObject(ObjectData objectData, PeerData peer_data_recv)
{
	PeerLedgerList::iterator peer_ledger_it;
	ObjectLedgerMap::iterator object_map_it;
	ObjectLedger *object_ledger;

	//Check whether the received object information is already stored in the super peer
	object_map_it = object_map.find(objectData.getKey());

	//If the object is not already known, have the iterator point to a new object instead.
	if (object_map_it == object_map.end())
	{
		//TODO: The fact that a duplicate key was received should be logged
		object_ledger = new ObjectLedger();

		//Log the file name and what peers it is stored on
		object_ledger->objectDataPtr = ObjectDataPtr(new ObjectData(objectData));

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
		peer_ledger.peerDataPtr = PeerDataPtr(new PeerData(peer_data_recv));
		peer_ledger.addObjectRef(object_ledger->objectDataPtr);
		peer_list.push_back(peer_ledger);

		object_ledger->addPeerRef(peer_ledger.peerDataPtr);	//Add a peer to the ObjectInfo object's peer vector

	} else {

		peer_ledger_it->addObjectRef(object_ledger->objectDataPtr);
		object_ledger->addPeerRef(peer_ledger_it->peerDataPtr);	//Add a peer to the ObjectInfo object's peer vector
	}

	if (object_map_it == object_map.end())
	{
		object_map.insert(std::make_pair(objectData.getKey(), *object_ledger));
		delete(object_ledger);
	}
}

unsigned int GroupLedger::getGroupSize()
{
	return peer_list.size();
}

PeerDataPtr GroupLedger::getPeerPtr(const int &i)
{
	return (peer_list.at(i)).peerDataPtr;
}
