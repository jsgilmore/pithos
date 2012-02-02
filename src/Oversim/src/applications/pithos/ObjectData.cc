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

ObjectData::ObjectData()
{
	size = 0;
	key = OverlayKey::UNSPECIFIED_KEY;
}

ObjectData::ObjectData(std::string name, int siz, OverlayKey k)
{
	object_name = name;
	size = siz;
	key = k;
}

ObjectData::~ObjectData()
{
	// TODO Auto-generated destructor stub
}

ObjectData& ObjectData::operator=(const ObjectData& other)
{
	if (&other==this)
		return *this;

	object_name = other.object_name;
	size = other.size;
	key = other.key;

	return *this;
}

bool operator==(const ObjectData& object1, const ObjectData& object2)
{
	if (object1.object_name != object2.object_name)
		return false;

	if (object1.size != object2.size)
			return false;

	if (object1.key != object2.key)
			return false;

	return true;
}

bool operator!=(const ObjectData& object1, const ObjectData& object2)
{
	return !(object1 == object2);
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

OverlayKey ObjectData::getKey()
{
	return key;
}
