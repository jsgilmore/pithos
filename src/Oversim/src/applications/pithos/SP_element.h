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

		/** The transport address of the peer (IP address and port) */
		TransportAddress address;

		/** The latitude of the super peer in the virtual world */
		double latitude;

		/** The longitude of the super peer in the virtual world */
		double longitude;
	public:
		SP_element();
		virtual ~SP_element();

		/**
		 * Set the TransportAddress
		 *
		 * @param adr A TransportAddress containing the IP and port information
		 */
		void setAddress(const TransportAddress &adr);

		/**
		 * Set the TransportAddress
		 *
		 * @param ip_str A string specifying the IP of the location
		 * @param port The port of the object location
		 */
		void setAddress(const char* ip, const int &port);

		TransportAddress getAddress();

		/**
		 * Set the position
		 *
		 * @param lati latitude
		 * @param longi longitude
		 */
		void setPosition(const double &lati, const double &longi);

		/**
		 * @returns latitide
		 */
		double getLatitude();
		double getLongitude();

};

#endif /* SP_ELEMENT_H_ */
