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

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#define SHA1_WIPE_VARIABLES

#include <omnetpp.h>
#include <iostream>
#include <exception>
#include <SHA1.h>

#include "UnderlayConfigurator.h"
#include "GlobalStatistics.h"
#include "GlobalNodeListAccess.h"
#include "BaseApp.h"
#include "DHTStorage.h"

#include "GameObject.h"
#include "packet_m.h"
#include "groupPkt_m.h"
#include "bootstrapPkt_m.h"

#include "DHTMessage_m.h"

/**
 * The Communicator class is the Tier1 class for Pithos and therefore the means by
 * which Pithos communicates with higher and lower layers, as well as the overlay,
 * UDP and TCP. All other Pithos modules that wish to send information to other layers
 * must to so through the Communicator.
 *
 * @author John Gilmore
 */

class Communicator : public BaseApp
{
	private:
		// statistics
		int numSent;              //number of packets sent
		int numReceived;          //number of packets received


	protected:
		virtual void handleMessage(cMessage *msg);

		// application routines
		void initializeApp(int stage);                 // called when the module is being created
		void finishApp();                              // called when the module is about to be destroyed
		void handleTimerEvent(cMessage* msg);          // called when we received a timer message
		void deliver(OverlayKey& key, cMessage* msg);  // called when we receive a message from the overlay
		void handleUDPMessage(cMessage* msg);          // called when we receive a UDP message
		void handleUpperMessage (cMessage *msg);

		/**
		 * The function that handles RPC request from the higher layer to store, retrieve and update objects.
		 *
		 * @param msg The RPC message containing the request type and data
		 */
		bool handleRpcCall(BaseCallMessage *msg);

		/**
		 * This function has not been implemented yet, but is meant to
		 * be a response to another node performing a push request to
		 * this node. The response would be whether the push was successful
		 * at the destination and will be sent back to the storing node.
		 */
		void handlePutRequest(DHTPutCall* dhtMsg);

		/**
		 * @see handlePutRequest()
		 */
		void handleGetRequest(DHTGetCall* dhtMsg);

		/**
		 * Called by @see handleRpcCall() when an internal RPC called requested a push be done.
		 *
		 * @param capiGetMsg Message containing the data required for the push
		 */
		void handleGetCAPIRequest(DHTgetCAPICall* capiGetMsg);

		/**
		 * @see handleGetCapiRequest()
		 */
		void handlePutCAPIRequest(DHTputCAPICall* capiPutMsg);

		void handleDumpDhtRequest(DHTdumpCall* call);
		void handleTraceMessage(cMessage* msg);

		void handleRpcResponse(BaseResponseMessage* msg, const RpcState& state, simtime_t rtt);

		/**
		 * Called when a message is received from a super peer. The message is inspected to generate the required response.
		 *
		 * @param msg Message received from the Super Peer
		 */
		void handleSPMsg(cMessage *msg);

		/**
		 * Called when a message is received from a group peer over UDP. The message is inspected to generate the required response.
		 *
		 * @param msg Message received from the Peer
		 */
		void handlePeerMsg(cMessage *msg);

		/**
		 * Route a message to the overlay for storage.
		 *
		 * @param msg Message containing the GameObject that should be stored.
		 */
		void overlayStore(cMessage *msg);

		/**
		 * Send a packet out over UDP to the destination IP address.
		 *
		 * @param msg The packet that should be sent.
		 */
		void sendPacket(cMessage *msg);

	public:
		Communicator() {};
		~Communicator() {};

		simtime_t getCreationTime();

		uint32_t externallySendInternalRpcCall(CompType destComp,
                BaseCallMessage* msg,
                cPolymorphic* context = NULL,
                simtime_t timeout = -1,
                int retries = 0,
                int rpcId = -1,
                RpcListener* rpcListener = NULL)
		{
			Enter_Method("externallySendInternalRpcCall()");	//Required for Omnet++ context switching between modules
			take(msg);	//This module should first take ownership of the received message before that message can be resent

			return sendInternalRpcCall(destComp, msg, context, timeout, retries, rpcId, rpcListener);
		}
};

#endif
