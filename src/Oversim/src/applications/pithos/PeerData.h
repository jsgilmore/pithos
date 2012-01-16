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

#ifndef PEERDATA_H_
#define PEERDATA_H_

#include <TransportAddress.h>
#include <tr1/memory>

class PeerData;

/**
 * PeerDataPtr is a typedef for the shared_ptr smart pointer type, which using
 * reference counting to ensure that memory is never freed while there are still
 * pointers pointing to it. It also automatically frees memory if no pointers are
 * pointing to it anymore. Their use is requried when creating pointers to dynamic
 * structures like vectors.
 */
typedef std::tr1::shared_ptr <PeerData> PeerDataPtr;

/**
 * Abstract data type that defines a particular peer in the network.
 * Currently, only the TransportAddress is stored, but this can be
 * expanded later.
 *
 * @author John Gilmore
 */
class PeerData
{
	private:

		TransportAddress address; /**< The transport address of the peer (IP address and port) */

	public:
		PeerData();
		PeerData(const PeerData& other);
		PeerData(const TransportAddress& address);
		virtual ~PeerData();
		PeerData& operator=(const PeerData& other);

		friend bool operator==(const PeerData& object1, const PeerData& object2);
		friend bool operator!=(const PeerData& object1, const PeerData& object2);

		/**
		 * Set the TransportAddress
		 *
		 * @param adr A TransportAddress object containing the IP and port information
		 */
		void setAddress(TransportAddress adr);

		/**
		 * Add the TransportAddress
		 *
		 * @param ip_str A string specifying the IP of the location
		 * @param port The port of the object location
		 */
		void setAddress(const char* ip_str, int port);

		TransportAddress getAddress();
};

#endif /* PEERDATA_H_ */
