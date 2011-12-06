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

#ifndef PEER_LIST_PKT_H_
#define PEER_LIST_PKT_H_

#include <vector>
#include "PithosMessages_m.h"

/**
 * Implementation of the peer list packet, which expands on the abstract
 * msg definition. This further implementation is required to add a std::vector
 * to the message definition, which is not supported by the msg definition
 * language.
 *
 * @author John Gilmore
 */
class PeerListPkt : public PeerListPkt_Base
{
	protected:

		std::vector<PeerData> peer_list; /**< The vector (list) of peers that all store a specific object */

	public:

		PeerListPkt(const char *name=NULL, int kind=0)
		{
			Packet(name, kind);
		}

		PeerListPkt(const PeerListPkt& other)
		{
			//Make sure this copy constructor correctly constructs the cPacket class, the constructor is not called here, because of too many levels of inheritance
			setName(other.getName());
			operator=(other);
		}

		PeerListPkt& operator=(const PeerListPkt& other)
		{
			if (&other==this) return *this;
			PeerListPkt_Base::operator=(other);
			peer_list = other.peer_list;
			return *this;
		}

		virtual PeerListPkt *dup() {return new PeerListPkt(*this);}

		/**
		 * Set the size of the peer_list vector
		 *
		 * @param size The number of elements that the vector should be able to contain
		 */
		virtual void setPeer_listArraySize(unsigned int size) {}

		/**
		 * Get the size of the peer_list vector
		 *
		 * @returns size The number of elements that the vector contains
		 */
		virtual unsigned int getPeer_listArraySize() const {return peer_list.size();}

		/**
		 * Retrieve a single peer's information from the list of peers
		 *
		 * @param k the position of the peer in the list
		 * @returns The peer information at the specified position
		 */
		virtual PeerData& getPeer_list(unsigned int k) {return peer_list.at(k);}

		/**
		 * Set the peer information for a particular element in the list
		 *
		 * @param k the position of the element
		 * @param alist the peer information to be inserted
		 */
		virtual void setPeer_list(unsigned int k, const PeerData& alist) {peer_list.at(k)=alist;}

		/**
		 * Add a single peer information item to the list vector
		 *
		 * @param alist the peer information to be added
		 */
		virtual void addToPeerList(const PeerData& alist) {peer_list.push_back(alist);}

		/**
		 * Delete all elements contained in the peer list
		 */
		virtual void clearPeerList() {peer_list.clear();}

		//Note: Ensure that the names of the above methods remain the same, since they are defined so in the abstract class as pure virtual methods
};

Register_Class(PeerListPkt);

#endif /* PEER_LIST_PKT_H_ */
