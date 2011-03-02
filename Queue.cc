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

#include "Queue.h"

Queue::Queue() {
	// TODO Auto-generated constructor stub

}

Queue::~Queue() {
	delete(event);
	delete(queue);
}

void Queue::initialize()
{
	queue = new cPacketQueue("queue");
	event = new cMessage("event");
}

void Queue::handleMessage(cMessage *msg)
{
	if (msg == event)
	{
			send(queue->pop(), "out");

			if (!queue->isEmpty())
				scheduleAt(gate("out")->getChannel()->getTransmissionFinishTime(), event);
	}
	else {
		cPacket *pkt = check_and_cast<cPacket *>(msg);

		queue->insert(pkt);

		if (gate("out")->getChannel()->isBusy())
		{
			//Schedule the module to send the next packet as soon as the previous packet finishes
			scheduleAt(gate("out")->getChannel()->getTransmissionFinishTime(), event);
		}
		else scheduleAt(simTime(), event);
	}
}
