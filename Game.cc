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

#include<string.h>

#include "Game.h"
#include "Storage.h"
#include "Pithos_m.h"

Game::Game() {
	// TODO Auto-generated constructor stub

}

Game::~Game() {
	delete(event);
}

void Game::initialize()
{
	writeTime_av = par("avWriteTime");
	objectSize_av = par("avObjectSize");

	event = new cMessage("event");
	scheduleAt(5.0, event);
}

void Game::handleMessage(cMessage *msg)
{
	if (msg == event)
	{
		//char packet_name[20];
		int peer_index = intuniform(0, 20);
		PithosMsg *write_msg = new PithosMsg("storageRequest");
		write_msg->setByteLength(exponential(objectSize_av));
		//write_msg->setName("Storage req");
		write_msg->setPayloadType(STORE_REQ);

		//Depending on the index, the message is either sent to an indexed peer, or the super peer.
		if (peer_index != 20)
			sendDirect(write_msg, getParentModule()->getSubmodule("peer", peer_index), "write");
		else sendDirect(write_msg, getParentModule()->getSubmodule("super_peer"), "write");

		scheduleAt(simTime()+exponential(writeTime_av), event);
	}
	else delete(msg);
}
