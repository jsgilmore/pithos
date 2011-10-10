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

enum SP_indeces {
    UNKNOWN = -1,
    THIS = -2
};

class Peer_logic: public cSimpleModule
{
	private:
		cMessage *event;

		char directory_ip[16];
		int directory_port;
		double latitude;
		double longitude;

		TransportAddress super_peer_address;

		// statistics
		int numSentForStore;              //number of packets sent
	public:
		Peer_logic();
		virtual ~Peer_logic();

		bool hasSuperPeer();
		TransportAddress getSuperPeerAddress();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);

		void sendObjectForStore(long long int o_size);
		void handleP2PMsg(cMessage *msg);
		void handleRequest(cMessage *msg);

		void OverlayStore(GameObject *go, std::vector<TransportAddress> send_list);

		void joinRequest(const TransportAddress &dest_adr);
};

Define_Module(Peer_logic);

#endif /* PEER_LOGIC_H_ */
