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
#include "Communicator.h"

#include "PeerListPkt.h"
#include "PithosMessages_m.h"
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

		enum PendingRpcsStates {
			INIT = 0,
			LOOKUP_STARTED = 1,
			GET_HASH_SENT = 2,
			GET_VALUE_SENT = 3,
			PUT_SENT = 4
		};

		/**
		 * This class provides a means to store all information about pending RPC sent to the communicator.
		 * A map of all pending RPCs are maintained and matched against RPC responses received. If a response for
		 * a pending RPC is received, the original caller can be informed about the outcome of the request.
		 *
		 * @author John Gilmore, Gregoire Menuel, Ingmar Baumgart
		 */
		class PendingRpcsEntry
		{
			public:
				PendingRpcsEntry()
				{
					getCallMsg = NULL;
					putCallMsg = NULL;
					numSent = 0;

					numGroupPutFailed = 0;
					numGroupPutSucceeded = 0;
					numDHTPutSucceeded = 0;
					numDHTPutFailed = 0;

					numGroupGetFailed = 0;
					numGroupGetSucceeded = 0;
					numDHTGetSucceeded = 0;
					numDHTGetFailed = 0;
				};

				RootObjectGetCAPICall* getCallMsg;
				RootObjectPutCAPICall* putCallMsg;

				TransportAddress group_address;

				int numSent;

				int numGroupPutFailed;
				int numGroupPutSucceeded;
				int numDHTPutSucceeded;
				int numDHTPutFailed;

				int numGroupGetFailed;
				int numGroupGetSucceeded;
				int numDHTGetSucceeded;
				int numDHTGetFailed;
		};

		//friend std::ostream& operator<<(std::ostream& Stream, const PendingRpcsEntry& entry);

		typedef std::map<uint32_t, PendingRpcsEntry> PendingRpcs;
		PendingRpcs pendingRpcs; /**< a map of all pending RPC operations */

		int replicas;	//The number of replicas group storage is set to.
		bool fastPut;

		bool disableDHT;

	public:
		Peer_logic();
		virtual ~Peer_logic();

		/**
		 * Handle a request from the higher layer for store
		 *
		 * @param capiPutMsg the request containing the GameObject
		 */
		void handlePutCAPIRequest(RootObjectPutCAPICall* capiPutMsg);

		/**
		 * Handle a request from the higher layer for retrieve
		 *
		 * @param capiGetMsg the request containing the OverlayKey
		 */
		void handleGetCAPIRequest(RootObjectGetCAPICall* capiGetMsg);

	protected:
		virtual void initialize();
		void finalize();
		virtual void handleMessage(cMessage *msg);

		void processPut(PendingRpcsEntry entry, ResponsePkt *response);

		void processGet(PendingRpcsEntry entry, ResponsePkt *response);

		void handleResponseMsg(cMessage *msg);
};

#endif /* PEER_LOGIC_H_ */
