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

#ifndef DIRECTORY_LOGIC_H_
#define DIRECTORY_LOGIC_H_

#include <omnetpp.h>
#include <iostream>
#include <vector>

#include "BaseApp.h"

#include "bootstrapPkt_m.h"
#include "groupPkt_m.h"

#include "SP_element.h"

/**
 * The Directory logic class, which is used by the Directory Server
 * Omnet module implements P2P bootstrapping of Pithos. It provides
 * a known IP address, where Super Peers may register their IP
 * addresses and locations. Peers can then query this service and
 * probide their own location whereupon the server will respond with
 * the IP of a Super Peer nearest to that location.
 *
 * @author John Gilmore
 */
class Directory_logic : public BaseApp
{
	private:

		std::vector<SP_element> sp_adr_list; /**< The list that holds the combination of Super Peer IPs and their positions in the world */

		simsignal_t superPeerNumSignal; /**< Signal for collecting statistics on the number of Super Peers in the directory. */

		simsignal_t noSuperPeersSignal; /**< Signal for collecting statistics on how many times a node requested to join the network when there were no Super Peers to reply with. */

	protected:

		/**
		 * Called when the module is being created
		 *
		 * @param stage Which stage of initialisation Oversim is in.
		 */
		void initializeApp(int stage);

		/** Called when the module is about to be destroyed */
		void finishApp();

		/**
		 * Called when a timer message is received
		 *
		 * @param msg The timer message
		 */
		void handleTimerEvent(cMessage* msg);

		/**
		 * Called when a message is received from the overlay
		 *
		 * @param key The routing key of the received overlay message
		 * @param msg The message received from the overlay
		 */
		void deliver(OverlayKey& key, cMessage* msg);

        /**
         * Called when a UDP message is received
         *
         * @param msg The received UDP message
         */
		void handleUDPMessage(cMessage* msg);

		/**
		 * Function called when a peer requests to join the network.
		 * The function replies with a Super Peer address which the
		 * peer should contact.
		 *
		 * @param boot_req Message containing the joining peer's location and address information.
		 */
		void handleJoinReq(bootstrapPkt *boot_req);

		/**
		 * Function that calculates the nearest super peer to the joining peer.
		 *
		 * @param lati Latitude of the joining peer.
		 * @param longi Longitude of the joining peer.
		 *
		 * @return The TransportAddress (IP and port) of the nearest super peer
		 */
		TransportAddress findAddress(const double &lati, const double &longi);

		/**
		 * Handles a request by a super peer to be added to the directory.
		 *
		 * @param boot_req The address and location of the super peer that wishes to be added to the directory.
		 */
		void handleSuperPeerAdd(bootstrapPkt *boot_req);

		/**
		 * Checks whether any super seers have been added to the Directory Server
		 *
		 * @return True if there are super seers, and false otherwise.
		 */
		bool superPeersExist();

	public:
		Directory_logic();
		~Directory_logic();
};

#endif /* DIRECTORY_LOGIC_H_ */
