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

#ifndef GROUPLEDGER_H_
#define GROUPLEDGER_H_

#include "ObjectLedger.h"
#include "PeerLedger.h"

class GroupLedger
{
	private:

		/**< A map that records all peers that belong to this peer's group */
		typedef std::vector<PeerLedger> PeerLedgerList;
		PeerLedgerList peer_list;

		/**< A map that records all objects information stored in this super peer's group */
		typedef std::map<OverlayKey, ObjectLedger> ObjectLedgerMap;
		ObjectLedgerMap object_map;

	public:
		GroupLedger();
		virtual ~GroupLedger();

		bool isObjectInGroup(OverlayKey key);
		bool isPeerInGroup(PeerData peerData);
		PeerDataPtr getRandomPeer(OverlayKey key);
		PeerDataPtr getRandomPeer();
		void addPeer(PeerData peer_dat);
		void addObject(ObjectData objectData, PeerData peer_data_recv);
		unsigned int getGroupSize();
		PeerDataPtr getPeerPtr(const int &i);
};

#endif /* GROUPLEDGER_H_ */
