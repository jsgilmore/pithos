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

#include "PeerData.h"

PeerData::PeerData() {
	// TODO Auto-generated constructor stub

}

PeerData::PeerData(const PeerData& other)
{
	operator=(other);
}

PeerData::~PeerData() {
	// TODO Auto-generated destructor stub
}

void PeerData::setAddress(TransportAddress adr)
{
	address = adr;
}

PeerData& PeerData::operator=(const PeerData& other)
{
	if (&other==this)
		return *this;

	address = other.address;

	return *this;
}

bool PeerData::operator==(const PeerData &other) const
{
	if (address == other.address)
		return true;
	else return false;
}

bool PeerData::operator!=(const PeerData &other) const {
   return !(*this == other);
 }

void PeerData::setAddress(const char* ip_str, int port)
{
	IPvXAddress ip;

	ip.set(ip_str);
	address.setIp(ip, port);
}

TransportAddress PeerData::getAddress()
{
	return address;
}
