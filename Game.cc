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

#include "Game.h"
#include "Storage.h"


Game::Game() {
	// TODO Auto-generated constructor stub

}

Game::~Game() {
	cancelAndDelete(event);
}

void Game::initialize()
{
	writeTime_av = par("avWriteTime");
	objectSize_av = par("avObjectSize");
	wait_time = par("wait_time");
	generationTime = par("generation_time");

	event = new cMessage("event");
}

void Game::sendRequest()
{
	char msgName [20];
	int64_t filesize = exponential(objectSize_av);
	Message *write_msg = new Message("storReq");
	sprintf(msgName, "store_req-%d", getParentModule()->getIndex());
	write_msg->setName(msgName);
	write_msg->setValue(filesize);
	write_msg->setPayloadType(STORE_REQ);
	send(write_msg, "gate$o");
	write_msg = NULL;
}

void Game::handleMessage(cMessage *msg)
{
	//This module is only connected to the peer_logic module, so we know exactly what we can expect to receive.
	//This module can receive two types of messages, but they have the same effect. An event that was scheduled can be received, but a cMessage is also expected from the peer_logic module to initiate request generation.
	//This message is received
	if (msg == event)
	{
		if (simTime() < generationTime + wait_time + join_time)	//This should ensure that the simulation ends.
		{
			sendRequest();

			scheduleAt(simTime()+exponential(writeTime_av), event);
		}
	}
	else if (strcmp(msg->getName(), "request_start") == 0)
	{
		join_time = simTime() - wait_time;		//Adding join time gives every game module the same time to send successful requests

		if (!event->isScheduled())
		{
			if (simTime() < generationTime + wait_time + join_time)
			{
				sendRequest();

				scheduleAt(simTime()+exponential(writeTime_av), event);
			}
		} else error("Duplicate request start messages received");

		delete(msg);
	}
	else error("Game received unknown message\n");
}
