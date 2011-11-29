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

#ifndef PEER_LOGIC_H_
#define PEER_LOGIC_H_

#include <omnetpp.h>

#include "BaseApp.h"
#include "GlobalNodeListAccess.h"

#include "GameObject.h"
#include "PeerData.h"

#include "groupPkt_m.h"
#include "PeerListPkt.h"
#include "bootstrapPkt_m.h"
#include "PithosTestMessages_m.h"

enum SP_indeces {
    UNKNOWN = -1,
    THIS = -2
};

/**
 * The implemented peer logic or peer intelligence. This includes joining
 * the network, and handling requests from the higher layer.
 *
 * @author John Gilmore
 */
class Peer_logic: public cSimpleModule
{
	private:

		cMessage *event; /**< An event used to trigger a join request */

		char directory_ip[16]; /**< The IP address of the directory server (specified as a Omnet param value) */

		int directory_port; /**< The port of the directory server (specified as a Omnet param value) */

		double latitude; /**< The latitude of this peer (position in the virtual world) */

		double longitude; /**< The longitude of this peer (position in the virtual world) */

		TransportAddress super_peer_address; /**< The TransPort address of the group super peer (this address is set, after the peer has joined a group) */

	public:
		Peer_logic();
		virtual ~Peer_logic();

		/**
		 * @returns true if a super peer has been set for this peer and false if not.
		 */
		bool hasSuperPeer();

		/**
		 * Handle a request from the higher layer for store, retrieve or update
		 *
		 * @param capiPutMsg the request containing the GameObject
		 */
		void handlePutCAPIRequest(RootObjectPutCAPICall* capiPutMsg);
		//void handlePutCAPIRequest(const GameObject &go);

		TransportAddress getSuperPeerAddress();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);

		/**
		 * Handle a message received from the group over UDP
		 *
		 * @param msg the message received
		 */
		void handleP2PMsg(cMessage *msg);

		/**
		 * Send a join request to the directory server or a super peer
		 *
		 * @param the destination address of the directory server of super peer
		 */
		void joinRequest(const TransportAddress &dest_adr);
};

#endif /* PEER_LOGIC_H_ */
