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

#include "ObjectInfo.h"

ObjectInfo::ObjectInfo() {
	// TODO Auto-generated constructor stub

}

ObjectInfo::~ObjectInfo() {
	// TODO Auto-generated destructor stub
}

void ObjectInfo::setObjectName(const std::string &o_name)
{
	object_name = o_name;
}

void ObjectInfo::setObjectName(char *o_name)
{

	object_name = o_name;
}

std::string ObjectInfo::getObjectName()
{
	return object_name;
}

void ObjectInfo::addPeerRef(PeerDataPtr peer_data_ptr)
{
	PeerDataPtr ptr(peer_data_ptr);
	location_list.push_back(ptr);
}

PeerDataPtr ObjectInfo::getPeerRef(const int &i)
{
	return location_list.at(i);
}

PeerDataPtr ObjectInfo::getRandPeerRef()
{
	int index = intuniform(0, location_list.size());
	return location_list.at(index);
}

bool ObjectInfo::isPeerPresent(PeerDataPtr peer_ptr)
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

int ObjectInfo::getPeerListSize()
{
	return location_list.size();
}

void ObjectInfo::setSize(const int &siz)
{
	size = siz;
}

int ObjectInfo::getSize()
{
	return size;
}
