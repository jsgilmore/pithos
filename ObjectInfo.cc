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

void ObjectInfo::addAddress(const TransportAddress &adr)
{
	location_list.push_back(adr);
}

void ObjectInfo::addAddress(const char* ip_str, int port)
{
	IPvXAddress ip;
	TransportAddress adr;

	ip.set(ip_str);
	adr.setIp(ip, port);
	location_list.push_back(adr);
}

TransportAddress ObjectInfo::getAddress(const int &i)
{
	return location_list.at(i);
}

TransportAddress ObjectInfo::getRandAddress()
{
	int index = intuniform(0, location_list.size());
	return location_list.at(index);
}

void ObjectInfo::setSize(const int &siz)
{
	size = siz;
}

int ObjectInfo::getSize()
{
	return size;
}
