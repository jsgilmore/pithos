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

#ifndef SP_ELEMENT_H_
#define SP_ELEMENT_H_

#include <TransportAddress.h>

class SP_element
{
	private:
		TransportAddress address;
		double latitude;
		double longitude;
	public:
		SP_element();
		virtual ~SP_element();
		void setAddress(TransportAddress adr);
		void setAddress(const char* ip, int port);
		TransportAddress getAddress();
		void setPosition(double lati, double longi);
		double getLattitude();
		double getLongitude();

};

#endif /* SP_ELEMENT_H_ */
