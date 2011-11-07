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

#include "SP_element.h"

SP_element::SP_element() {
	// TODO Auto-generated constructor stub

}

SP_element::~SP_element() {
	// TODO Auto-generated destructor stub
}

void SP_element::setAddress(const TransportAddress &adr)
{
	address = adr;
}

void SP_element::setAddress(const char* ip_str, const int &port)
{
	IPvXAddress ip;

	ip.set(ip_str);
	address.setIp(ip, port);
}

TransportAddress SP_element::getAddress()
{
	return address;
}

void SP_element::setPosition(const double &lati, const double &longi)
{
	latitude = lati;
	longitude = longi;
}

double SP_element::getLatitude()
{
	return latitude;
}

double SP_element::getLongitude()
{
	return longitude;
}
