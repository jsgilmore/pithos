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

#include "ObjectData.h"

const ObjectData ObjectData::UNSPECIFIED_OBJECT;

ObjectData::ObjectData(std::string name, int siz, OverlayKey k, simtime_t time, int t, int group_size)
{
	object_name = name;
	size = siz;
	key = k;
	creationTime = time;
	ttl = t;
	init_group_size = group_size;
	max_group_size = group_size;	//The maximum group size when the object is created is equal to the initial group size
}

ObjectData::ObjectData(const GameObject& go, int group_size)
{
	object_name = go.getObjectName();
	size = go.getSize();
	key = go.getHash();
	creationTime = go.getCreationTime();
	ttl = go.getTTL();
	init_group_size = group_size;
	max_group_size = group_size;
}

ObjectData::ObjectData(const ObjectData& other)
{
	operator=(other);
}

ObjectData::~ObjectData()
{
}

ObjectData& ObjectData::operator=(const ObjectData& other)
{
	if (&other==this)
		return *this;

	object_name = other.object_name;
	size = other.size;
	key = other.key;
	creationTime = other.creationTime;
	ttl = other.ttl;
	init_group_size = other.init_group_size;
	max_group_size = other.max_group_size;

	return *this;
}

bool operator==(const ObjectData& object1, const ObjectData& object2)
{
	if (object1.key != object2.key)
		return false;

	//Other comparisons significantly slow down computation and keys already define a unique object

	return true;
}

bool operator!=(const ObjectData& object1, const ObjectData& object2)
{
	return !(object1 == object2);
}

std::ostream& operator<<(std::ostream& stream, const ObjectData object_data)
{
    return stream << /*This will state the node number and object number: */ object_data.object_name
					<< " Size: " << object_data.size
                  << " Key: " << object_data.key
				  << " Creation time: " << object_data.creationTime
				  << " TTL: " << object_data.ttl;
}

void ObjectData::setObjectName(const std::string &o_name)
{
	object_name = o_name;
}

void ObjectData::setObjectName(char *o_name)
{

	object_name = o_name;
}

std::string ObjectData::getObjectName()
{
	return object_name;
}

void ObjectData::setSize(const int &siz)
{
	size = siz;
}

int ObjectData::getSize()
{
	return size;
}

void ObjectData::setKey(const OverlayKey &k)
{
	key = k;
}

void ObjectData::setInitGroupSize(const int &siz)
{
	init_group_size = siz;
}

int ObjectData::getInitGroupSize()
{
	return init_group_size;
}

void ObjectData::setMaxGroupSize(const int &siz)
{
	max_group_size = siz;
}

int ObjectData::getMaxGroupSize()
{
	return max_group_size;
}

OverlayKey ObjectData::getKey()
{
	return key;
}

void ObjectData::setCreationTime(simtime_t time)
{
	creationTime = time;
}

simtime_t ObjectData::getCreationTime()
{
	return creationTime;
}

void ObjectData::setTTL(int t)
{
	ttl = t;
}

int ObjectData::getTTL()
{
	return ttl;
}

bool ObjectData::isUnspecified()
{
	return (*this == ObjectData::UNSPECIFIED_OBJECT);
}
