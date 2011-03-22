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
	network_size = par("network_size");

	//Initialise queue statistics collection
	busySignal = registerSignal("busy");
	emit(busySignal, 0);
}

void Peer_logic::handleRequest(cMessage *msg)
{
	Message *m = check_and_cast<Message *>(msg);
	EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received store request of size " << m->getValue() << "\n";

	sendObjectForStore(m->getValue());
}

void Peer_logic::handleStore(cMessage *msg)
{
	PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);
	cMessage *storage_msg = new cMessage("storage");

	GameObject *go = (GameObject *)pithos_m->removeObject("GameObject");

	if (go->getType() == ROOT)
		EV << getName() << " " << getIndex() << " received root Game Object of size " << go->getSize() << "\n";
	else if (go->getType() == REPLICA)
		EV << getName() << " " << getIndex() << " received replica Game Object of size " << go->getSize() << "\n";

	storage_msg->addObject(go);
	send(storage_msg, "write");
}

void Peer_logic::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "request") == 0)
	{
		handleRequest(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "in") == 0)
	{
		handleStore(msg);
	}
	else {
		EV << "Illegal message received\n";
	}

	delete(msg);
}

void Peer_logic::sendObjectForStore(int64_t o_size)
{
	int i;
	int replicas = par("replicas_g");
	int storage_node_index;
	simtime_t sendDelay;
	PithosMsg *write;
	PithosMsg *write_dup;
	GameObject *go;
	GameObject *go_dup;

	//Create the packet that will house the game object
	write = new PithosMsg("write");
	write->setByteLength(o_size);
	write->setPayloadType(WRITE);

	go = new GameObject("GameObject");
	go->setSize(o_size);

	//Send the message to be stored on the specified number of replicas.
	for (i = 0 ; i < replicas ; i++)
	{
		go_dup = go->dup();
		write_dup = write->dup();

		if (i > 0) go_dup->setType(REPLICA);

		write_dup->addObject(go_dup);

		storage_node_index = intuniform(0, network_size-2);	//19. because there are 20 nodes and a super peer, which means 20 connections, which means 0-19 values.

		if (gate("out", storage_node_index)->getTransmissionChannel()->isBusy())
		{
			sendDelay = gate("out", storage_node_index)->getTransmissionChannel()->getTransmissionFinishTime()-simTime();
			sendDelayed(write_dup, sendDelay, "out", storage_node_index);
			emit(busySignal, 1);
		}
		else {
			send(write_dup, "out", storage_node_index);
			emit(busySignal, 0);
		}
	}
	delete(write);
	delete(go);
}
