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
#include "ObjectInfo.h"

#include "Message_m.h"
#include "PeerListPkt.h"
#include "PeerData.h"
#include "overlayPkt_m.h"

class Super_peer_logic : public cSimpleModule
{
	private:
		cMessage *event;

		double latitude;
		double longitude;
		char directory_ip[16];
		int directory_port;

		std::vector<PeerData> group_peers;
		std::vector<ObjectInfo> object_list;

		//Statistics
		simsignal_t groupSizeSignal;
		simsignal_t OverlayWriteSignal;
		simsignal_t OverlayDeliveredSignal;
		simsignal_t joinTimeSignal;

		simsignal_t overlaysStoreFailSignal;
	public:
		Super_peer_logic();
		virtual ~Super_peer_logic();
	protected:
		int network_size;

		void initialize();                 // called when the module is being created
		void finish();                              // called when the module is about to be destroyed

		virtual void handleMessage(cMessage *msg);
		void handleOverlayWrite(PeerListPkt *plist_p);
		void handleP2PMsg(cMessage *msg);
		void handleBootstrapPkt(cMessage *msg);
		void GroupStore(overlayPkt *overlay_p);
		void addSuperPeer();
};

Define_Module(Super_peer_logic);

#endif /* PEER_LOGIC_H_ */
