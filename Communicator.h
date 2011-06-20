//
// Copyright (C) 2009 Institut fuer Telematik, Universitaet Karlsruhe (TH)
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

/**
 * @author Antonio Zea
  */

#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#define SHA1_WIPE_VARIABLES

#include <omnetpp.h>
#include <iostream>
#include <exception>
#include <SHA1.h>

#include "UnderlayConfigurator.h"
#include "GlobalStatistics.h"
#include "GlobalNodeListAccess.h"
#include "BaseApp.h"

#include "GameObject.h"
#include "packet_m.h"
#include "groupPkt_m.h"
#include "bootstrapPkt_m.h"

#include "DHTMessage_m.h"

class Communicator : public BaseApp
{
	private:
		// statistics
		int numSent;              //number of packets sent
		int numReceived;          //number of packets received


		virtual void handleMessage(cMessage *msg);

		// application routines
		void initializeApp(int stage);                 // called when the module is being created
		void finishApp();                              // called when the module is about to be destroyed
		void handleTimerEvent(cMessage* msg);          // called when we received a timer message
		void deliver(OverlayKey& key, cMessage* msg);  // called when we receive a message from the overlay
		void handleUDPMessage(cMessage* msg);          // called when we receive a UDP message
		void handleUpperMessage (cMessage *msg);

		bool handleRpcCall(BaseCallMessage *msg);
		void handlePutRequest(DHTPutCall* dhtMsg);
		void handleGetRequest(DHTGetCall* dhtMsg);
		void handleGetCAPIRequest(DHTgetCAPICall* capiGetMsg);
		void handlePutCAPIRequest(DHTputCAPICall* capiPutMsg);
		void handleDumpDhtRequest(DHTdumpCall* call);

		void handleSPMsg(cMessage *msg);
		void handlePeerMsg(cMessage *msg);
		void overlayStore(cMessage *msg);
		void sendPacket(cMessage *msg);

	public:
		Communicator() {};
		~Communicator() {};
};


#endif
