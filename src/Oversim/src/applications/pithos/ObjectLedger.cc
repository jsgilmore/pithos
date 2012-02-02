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

#include "ObjectLedger.h"

ObjectLedger::ObjectLedger() {
	// TODO Auto-generated constructor stub

}

ObjectLedger::~ObjectLedger() {
	location_list.clear();
}

void ObjectLedger::addPeerRef(PeerDataPtr peer_data_ptr)
{
	if (!isPeerPresent(peer_data_ptr))
	{
		PeerDataPtr ptr(peer_data_ptr);
		location_list.push_back(ptr);
	}
}

PeerDataPtr ObjectLedger::getPeerRef(const int &i)
{
	return location_list.at(i);
}

PeerDataPtr ObjectLedger::getRandPeerRef()
{
	int index = intuniform(0, location_list.size());
	return location_list.at(index);
}

bool ObjectLedger::isPeerPresent(PeerDataPtr peer_ptr)
{
	std::vector<PeerDataPtr>::iterator list_ptr;

	for (list_ptr = location_list.begin() ; list_ptr != location_list.end() ; list_ptr++)
	{
		//This checks whether the PeerData object have the same values
		if (**list_ptr == *peer_ptr)
			return true;
	}

	return false;
}

unsigned int ObjectLedger::getPeerListSize()
{
	return location_list.size();
}

void ObjectLedger::erasePeerRef(const int &i)
{
	location_list.erase(location_list.begin()+i);
}

void ObjectLedger::erasePeerRef(PeerDataPtr peer_data_ptr)
{
	bool found = false;
	for (unsigned int i = 0 ; i < location_list.size() ; i++)
	{
		if (peer_data_ptr == location_list.at(i))
		{
			found = true;
			location_list.erase(location_list.begin()+i);
			break;
		}
	}

	if (!found)
		opp_error("Peer pointer not found when erasing.");
}
