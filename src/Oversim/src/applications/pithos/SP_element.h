//
// Copyright (C) 2011 MIH Media lab, University of Stellenbosch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef SP_ELEMENT_H_
#define SP_ELEMENT_H_

#include <TransportAddress.h>

/**
 * The abstract data type of a super peer element used in the directory server.
 * This class stores the information of a single super peer, consisting of transport address,
 * and position. The class forms part of a std::vector in the directory server, which stors
 * the information of many super peers.
 *
 * @author John Gilmore
 */
class SP_element
{
	private:
		TransportAddress address;
		double latitude;
		double longitude;
	public:
		SP_element();
		virtual ~SP_element();
		void setAddress(const TransportAddress &adr);
		void setAddress(const char* ip, const int &port);
		TransportAddress getAddress();
		void setPosition(const double &lati, const double &longi);
		double getLattitude();
		double getLongitude();

};

#endif /* SP_ELEMENT_H_ */
