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

typedef std::vector<PeerLedger> PeerLedgerList;
typedef std::map<OverlayKey, ObjectLedger> ObjectLedgerMap;

class GroupLedger : public cSimpleModule
{
	private:

		void initialize();
	    void handleMessage(cMessage* msg);
	    void finish();

		/**< A map that records all peers that belong to this peer's group */
		PeerLedgerList peer_list;

		/**< A map that records all objects information stored in this super peer's group */
		ObjectLedgerMap object_map;

		GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node*/

		static const int TEST_MAP_INTERVAL = 10; /**< interval in seconds for writing periodic statistical information */

		cMessage *periodicTimer; /**< timer self-message for writing periodic statistical information */

		int numPeerRemoveFail;
		int numPeerRemoveSuccess;
		int numObjectRemoveFail;
		int numObjectRemoveSuccess;
		int numObjectGetFail;
		int numObjectGetSuccess;
		int numPeerKnownError;
		int numPeerKnownSuccess;

	public:
		GroupLedger();
		virtual ~GroupLedger();

		/**
		 * Checks whether a given object key exists in the ledger.
		 *
		 * @param key The hash value of the object in question
		 *
		 */
		bool isObjectInGroup(OverlayKey key);

		/**
		 * Checks whether a given peer exists in the ledger.
		 *
		 * @param peerData The peer data of the peer in question (containing its transport address)
		 */
		bool isPeerInGroup(PeerData peerData);

		/**
		 * A debugging function to verify whether a given object was stored correctly on a given peer.
		 * This function checks both the peer and object lists to verify the pointers in both directions.
		 * It should be noted that this function if very expensive and should only be used for debugging.
		 *
		 * @param object_data The object data to be checked
		 * @param peer_data The peer data containing the peer's transport address
		 */
		bool isObjectOnPeer(ObjectData object_data, PeerData peer_data);

		/**
		 * The functions returns a random peer in the ledger
		 *
		 * @return A smart pointer to a random peer in the ledger
		 */
		PeerDataPtr getRandomPeer();

		/**
		 *	This functions returns a random peer that hosts the specified object.
		 *
		 *	@param key A key hash of the object that the peer should contain
		 *
		 *	@return A smart pointer to a peer containing the specified object
		 */
		PeerDataPtr getRandomPeer(OverlayKey key);

		/**
		 * Add a peer to the ledger
		 *
		 * @param peer_dat The data of the peer to be added
		 */
		void addPeer(PeerData peer_dat);

		/**
		 * Add an object to the ledger. An object has to exists on a peer and, therefore, the peer information that houses the object should also be specified.
		 * Both the object and the peer can be known or unknown. If an unknown peer is specified, it will also be added to the ledger. The only exception is that
		 * a peer that was already linked to a specific object may not again be linked to that object.
		 *
		 * @param objectData The data of the object to be added and/or linked.
		 * @param peer_data_recv The data of the peer to be added and/or linked.
		 */
		void addObject(ObjectData objectData, PeerData peer_data_recv);

		/**
		 * @return the number of peers recorded in the ledger.
		 */
		unsigned int getGroupSize();

		/**
		 * @return the number of objects recorded in the ledger
		 */
		unsigned int getNumGroupObjects();

		/**
		 * Retrieve the information of a peer at a specific location in the peer list
		 *
		 * @param i The index where the peer should be retrieved.
		 * @return A smart pointer to the peer at the specified index.
		 */
		PeerDataPtr getPeerPtr(const int &i);

		/**
		 * Remove a peer from the ledger.
		 * All object references to this peer will also be removed. If an object contains no more peer references, the object will also be removed.
		 *
		 * @param peer_data The data of the peer to be removed.
		 */
		void removePeer(PeerData peer_dat);

		/**
		 * Remove an object from the ledger. All peer references to this object will also be removed.
		 * If a peer contains no more peer references, the peer will NOT be removed, since a peer can exist with no objects housed on it..
		 *
		 * @param peer_data The data of the peer to be removed.
		 */
		void removeObject(OverlayKey key);

		/**
		 * @return the begin() iterator of the object map to allow for iteration over all objects stored.
		 */
		ObjectLedgerMap::iterator getObjectMapBegin();

		/**
		 * @return the end() iterator of the object map to allow for iteration over all objects stored.
		 */
		ObjectLedgerMap::iterator getObjectMapEnd();

		/**
		 * Checks whether this ledger is attached to a peer or a super peer.
		 *
		 * @return true if the ledger is attached to a super peer, and false if it is attached to a peer.
		 */
		bool isSuperPeerLedger();

		/**
		 * Record all statistics for the current group in which the ledger existed.
		 * Clear all recorded statistics and remove all peer and object data from the ledger.
		 */
		void recordAndClear();
};

#endif /* GROUPLEDGER_H_ */
