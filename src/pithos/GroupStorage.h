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

#ifndef GROUPSTORAGE_H_
#define GROUPSTORAGE_H_

#include <omnetpp.h>

#include "BaseApp.h"
#include "Peer_logic.h"

#include "GameObject.h"
#include "PeerListPkt.h"
#include "groupPkt_m.h"


class GroupStorage : public cSimpleModule
{
	public:
	GroupStorage();
		virtual ~GroupStorage();
	private:
		std::vector<PeerData> group_peers;

		simsignal_t groupSizeSignal;
		simsignal_t groupSendFailSignal;
		simsignal_t joinTimeSignal;

		void createWritePkt(groupPkt **write);
		void updateSuperPeerObjects(const char *objectName, unsigned long objectSize, std::vector<TransportAddress> send_list);
		void selectDestination(TransportAddress *dest_adr, std::vector<TransportAddress> send_list);
		void store(GameObject *go);
		void addPeers(cMessage *msg);
		int getReplicaNr();
	protected:

		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
};

Define_Module(GroupStorage);

#endif /* OVERLAYSTORAGE_H_ */
