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
#include "ObjectInfo.h"

#include "PeerListPkt.h"
#include "PeerData.h"
#include "overlayPkt_m.h"

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
		simsignal_t storeNumberSignal;
		simsignal_t overlayNumberSignal;

		simsignal_t overlaysStoreFailSignal;
	public:
		Super_peer_logic();
		virtual ~Super_peer_logic();
	protected:
		int network_size;

		void initialize();                 // called when the module is being created
		void finish();                              // called when the module is about to be destroyed

		virtual void handleMessage(cMessage *msg);
		void handleOverlayWrite(cMessage *msg);
		void handleP2PMsg(cMessage *msg);
		void handleJoinReq(cMessage *msg);
		void GroupStore(overlayPkt *overlay_p);
		void addSuperPeer();
		void addObject(cMessage *msg);
};

Define_Module(Super_peer_logic);

#endif /* PEER_LOGIC_H_ */
