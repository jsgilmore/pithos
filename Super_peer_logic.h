//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef SUPER_PEER_LOGIC_H_
#define SUPER_PEER_LOGIC_H_

#include <omnetpp.h>

#include "GameObject.h"
#include "Peer_logic.h"
#include "OverlayKey.h"

#include "Message_m.h"
#include "groupPkt_m.h"

class Super_peer_logic : public cSimpleModule
{
	private:
		void sp_identify();

		int largestKey;           // we'll store the "largestKey" parameter here for the Overlay
	public:
		Super_peer_logic();
		virtual ~Super_peer_logic();
	protected:
		int network_size;
		simsignal_t OverlayWriteSignal;

		void initialize();                 // called when the module is being created
		void finish();                              // called when the module is about to be destroyed

		virtual void handleMessage(cMessage *msg);
		void handleOverlayWrite(groupPkt *group_p);
		void handleP2PMsg(cMessage *msg);
		void handleBootstrapPkt(cMessage *msg);
};

Define_Module(Super_peer_logic);

#endif /* PEER_LOGIC_H_ */
