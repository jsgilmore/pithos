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
	group_peers.clear();
}

bool GroupLedger::isObjectInGroup(OverlayKey key)
{
	return object_map.find(key) == object_map.end();
}

bool GroupLedger::isPeerInGroup(TransportAddress address)
{
	return peer_map.find(address) == peer_map.end();
}

PeerDataPtr GroupLedger::getRandomPeer(OverlayKey key)
{
	ObjectLedgerMap::iterator object_map_it;
	ObjectLedger object_ledger_entry;
	PeerDataPtr peer_ptr;

	object_map_it = object_map.find(key);
	if (object_map_it == object_map.end())
		error("Object could not be found in group.");

	object_ledger_entry = object_map_it->second;

	peer_ptr = object_ledger_entry.getPeerRef(intuniform(0, peer_list_size-1));

	return peer_ptr;
}

PeerDataPtr GroupLedger::getRandomPeer()
{
    if (peer_list.size() == 0)
        error("No peers in group.");

    return (peer_list.at(intuniform(0, peer_map.size()-1))).peerDataPtr;
}

void GroupLedger::addPeer(PeerData peer_dat)
{
	bool found = false;

	//First check whether this peer is already known
	for (unsigned int j = 0 ; j < peer_list.size() ; j++)
	{
		if (*(peer_list.at(j).peerDataPtr) == peer_dat)
		{
			found = true;
			break;
		}
	}

	//If the peer is not known, add it to the peer list
	if (!found)
	{
		PeerDataPtr peer_dat_ptr(new PeerData(peer_dat));
		peer_list.push_back(peer_dat_ptr);
	}
}

void GroupLedger::addObject(ObjectData objectData, PeerData peer_data_recv)
{
	PeerLedgerList::iterator peer_ledger_it;

	for (peer_ledger_it = peer_list.begin() ; peer_ledger_it != peer_list.end() ; peer_ledger_it++)
	{
		//*peer_it returns a PeerDataPtr type, which again has to be dereferenced to obtain the PeerData object (**peer_it)
		if (**peer_it == peer_data_recv)
			break;
	}

	//If an object is stored on an unknown peer, first add that peer to the peer list
	if (peer_it == peer_list.end())
	{
		//TODO: Log this exception
		PeerDataPtr peer_dat_ptr(new PeerData(peer_data_recv));
		peer_list.push_back(peer_dat_ptr);

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

int GroupLedger::getGroupSize()
{
	return peer_map.size();
}

PeerDataPtr GroupLedger::getPeerPtr(const int &i)
{
	return (peer_list.at(i)).peerDataPtr;
}
