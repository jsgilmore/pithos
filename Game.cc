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

void Game::initializeApp(int stage)
{
	if (stage != MIN_STAGE_APP) return;

	writeTime_av = par("avWriteTime");
	objectSize_av = par("avObjectSize");
	wait_time = par("wait_time");
	generationTime = par("generation_time");

	event = new cMessage("event");
}

void Game::finishApp()
{

}

void Game::handleTimerEvent(cMessage* msg)
{
	if (msg == event)
	{
		if (simTime() < generationTime + wait_time + join_time)	//This should ensure that the simulation ends.
		{
			sendRequest();

			scheduleAt(simTime()+exponential(writeTime_av), event);
		}
	} else error("Game received unknown timer message\n");
}

void Game::handleLowerMessage (cMessage *msg)
{
	if (strcmp(msg->getName(), "request_start") == 0)
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

void Game::deliver(OverlayKey& key, cMessage* msg)
{
	error("Game module received overlay message, which should be impossible");
	return;	//We're not expecting anything here
}

void Game::sendRequest()
{
	char msgName [20];
	int64_t filesize = exponential(objectSize_av);
	groupPkt *write_pkt = new groupPkt();
	sprintf(msgName, "store_req-%d", getParentModule()->getIndex());
	write_pkt->setName(msgName);
	write_pkt->setValue(filesize);
	write_pkt->setPayloadType(STORE_REQ);
	send(write_pkt, "to_lowerTier");
}
