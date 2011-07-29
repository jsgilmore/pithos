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

#ifndef PEER_LIST_PKT_H_
#define PEER_LIST_PKT_H_

#include <vector>
#include "PeerListPkt_m.h"
//#include "packet_m.h"

class PeerListPkt : public PeerListPkt_Base
{
	protected:
		std::vector<PeerData> peer_list;
	public:
		//PeerListPkt(const char *name=NULL, int kind=0) : PeerListPkt_Base(name, kind) {};
		PeerListPkt(const char *name=NULL, int kind=0)
		{
			Packet(name, kind);
		}

		PeerListPkt(const PeerListPkt& other)	//TODO: Make sure this copy constructor correctly constructs the cPacket class
		{
			setName(other.getName());
			operator=(other);
		}
		//PeerListPkt(const PeerListPkt& other) : PeerListPkt_Base(other.getName()) { operator=(other); }

		PeerListPkt& operator=(const PeerListPkt& other)
		{
			if (&other==this) return *this;
			PeerListPkt_Base::operator=(other);
			peer_list = other.peer_list;
			return *this;
		}

		virtual PeerListPkt *dup() {return new PeerListPkt(*this);}

		//The names of these methods should remain the same, since they are defined so in the abstract class as pure virtual methods
		virtual void setPeer_listArraySize(unsigned int size) {}
		virtual unsigned int getPeer_listArraySize() const {return peer_list.size();}
		virtual PeerData& getPeer_list(unsigned int k) {return peer_list[k];}
		virtual void setPeer_list(unsigned int k, const PeerData& alist) {peer_list[k]=alist;}

		virtual void addToPeerList(const PeerData& alist) {peer_list.push_back(alist);}
		virtual void clearPeerList() {peer_list.clear();}
};

Register_Class(PeerListPkt);

#endif /* PEER_LIST_PKT_H_ */
