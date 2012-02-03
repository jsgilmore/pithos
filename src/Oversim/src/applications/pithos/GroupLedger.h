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

#include <GlobalStatistics.h>
#include <GlobalStatisticsAccess.h>
#include <DHTTestAppMessages_m.h>

#include "ObjectLedger.h"
#include "Communicator.h"
#include "GroupStorage.h"
#include "PeerLedger.h"

class GroupLedger : public cSimpleModule
{
	private:

		void initialize();
	    void handleMessage(cMessage* msg);
	    void finish();

		/**< A map that records all peers that belong to this peer's group */
		typedef std::vector<PeerLedger> PeerLedgerList;
		PeerLedgerList peer_list;

		/**< A map that records all objects information stored in this super peer's group */
		typedef std::map<OverlayKey, ObjectLedger> ObjectLedgerMap;
		ObjectLedgerMap object_map;

		GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node*/

		static const int TEST_MAP_INTERVAL = 10; /**< interval in seconds for writing periodic statistical information */

		cMessage *periodicTimer; /**< timer self-message for writing periodic statistical information */

		int numPeerGetFail;
		int numObjectGetFail;
		int numPeerKnownError;

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
		void removePeer(PeerData peer_dat);
		void removeObject(OverlayKey key);
};

#endif /* GROUPLEDGER_H_ */
