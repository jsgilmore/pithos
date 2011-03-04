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
		char msgName [20];
		int64 filesize = exponential(objectSize_av);
		cMessage *write_msg = new cMessage("storReq");
		sprintf(msgName, "store_req-%d", getParentModule()->getIndex());
		write_msg->setName(msgName);
		write_msg->setKind(filesize);

		send(write_msg, "write");
		write_msg = NULL;

		scheduleAt(simTime()+exponential(writeTime_av), event);
	}
	else delete(msg);
}
