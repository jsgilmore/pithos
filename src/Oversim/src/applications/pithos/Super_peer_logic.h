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

#ifndef SUPER_PEER_LOGIC_H_
#define SUPER_PEER_LOGIC_H_

#include <omnetpp.h>

#include "GameObject.h"
#include "Peer_logic.h"
#include "OverlayKey.h"
#include "GroupLedger.h"

#include "PeerListPkt.h"
#include "PeerData.h"
#include "PithosMessages_m.h"

/**
 * The implemented super peer logic or super peer intelligence.
 * This includes joining the directory server, allow peers to
 * join the group and maintaining a list of objects stored in
 * a group and updating peers about new objects.
 *
 * @author John Gilmore
 */
class Super_peer_logic : public cSimpleModule
{
	private:

		cMessage *event; /**< Event that is used to trigger the super peer informing the directory server of its existence */

		double latitude; /**< The latitude of the super peer in the virtual world */

		double longitude; /**< The longitude of the super peer in the virtual world */

		char directory_ip[16]; /**< The IP of the directory server */

		int directory_port; /**< The port of the directory server */

		PeerData lastPeerLeft;		/**< The peer data of the last peer that left the group */
		PeerData lastPeerJoined;	/**< The peer data of the last peer that joined the group */

		GroupLedger *group_ledger;

		simsignal_t groupSizeSignal; /**< A signal that records the group size over time */

		simsignal_t OverlayWriteSignal; /**< A signal that records the number of overlay writes performed */

		simsignal_t OverlayDeliveredSignal; /**< A signal that records the number of overlay objects successfully delivered */

		simsignal_t joinTimeSignal; /**< A signal that records when this super peer was listed in the directory server */

		simsignal_t storeNumberSignal; /**< A signal that records the number of objects stored in this group */

		simsignal_t overlayNumberSignal; /**< A signal that records the number of overlay objects stored in this group */

		simsignal_t overlaysStoreFailSignal; /**< A signal that records the number of times an overlay object could not be stored */
	public:
		Super_peer_logic();
		virtual ~Super_peer_logic();
	protected:

		/** Called when the module is being created */
		void initialize();

		/** Called when the module is about to be destroyed */
		void finish();

		virtual void handleMessage(cMessage *msg);

		/**
		 * Push the message received from the group to the overlay
		 *
		 * @param msg the message containing the data that should be pushed
		 */
		void handleOverlayWrite(cMessage *msg);

		/**
		 * Handle a message received from the group over UDP
		 *
		 * @param msg the message that should be handled
		 */
		void handleP2PMsg(cMessage *msg);

		void informGroupPeers(bootstrapPkt *boot_req, TransportAddress sourceAdr);

		void informJoiningPeer(bootstrapPkt *boot_req, TransportAddress sourceAdr);

		/**
		 * Handle a peer wishing to join the group
		 *
		 * @param msg the message containing the joining peer's information
		 */
		void handleJoinReq(cMessage *msg);

		/** Send a message to add this super peer to the directory server */
		void addSuperPeer();

		/** Add a new group object to the super peer's object list. */
		void addObject(cMessage *msg);

		void informLastJoinedOfLastLeft();
};

Define_Module(Super_peer_logic);

#endif /* PEER_LOGIC_H_ */
