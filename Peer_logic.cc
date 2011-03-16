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

#include "Peer_logic.h"

Peer_logic::Peer_logic()
{
}

Peer_logic::~Peer_logic()
{
}

void Peer_logic::initialize()
{
	//Initialise queue statistics collection
	busySignal = registerSignal("busy");
	emit(busySignal, 0);
}

void Peer_logic::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "request") == 0)
	{
		Message *m = check_and_cast<Message *>(msg);
		EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received store request of size " << m->getValue() << "\n";

		sendObjectForStore(m->getValue());

		delete(m);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "in") == 0)
	{
		PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);
		go *game_object = new go("game_object");
		cMessage *storage_msg = new cMessage("storage");

		EV << getName() << " " << getIndex() << " received write command of size " << pithos_m->getByteLength() << "\n";

		game_object->setSize(pithos_m->getByteLength());
		storage_msg->addObject(game_object);
		send(storage_msg, "write");

		delete(pithos_m);
	}
	else {
		EV << "Illegal message received\n";
		delete(msg);
	}
}

void Peer_logic::sendObjectForStore(int64_t o_size)
{
	int storage_node_index = intuniform(0, 18);	//18. because there are 20 nodes, which means 19 connections, which means 0-18 values.
	simtime_t sendDelay = gate("out", storage_node_index)->getTransmissionChannel()->getTransmissionFinishTime()-simTime();

	PithosMsg *write = new PithosMsg("write");
	write->setByteLength(o_size);
	write->setPayloadType(WRITE);

	if (gate("out", storage_node_index)->getTransmissionChannel()->isBusy())
	{
		sendDelayed(write, sendDelay, "out", storage_node_index);
		emit(busySignal, 1);
	}
	else {
		send(write, "out", storage_node_index);
		emit(busySignal, 0);
	}
}
