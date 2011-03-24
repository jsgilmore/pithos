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
	super_peer_index = UNKNOWN;
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

void Peer_logic::handleP2PMsg(cMessage *msg)
{
	PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

	if (pithos_m->getPayloadType() == WRITE)
	{
		cMessage *storage_msg = new cMessage("storage");
		GameObject *go = (GameObject *)pithos_m->removeObject("GameObject");

		if (go->getType() == ROOT)
			EV << getName() << " " << getIndex() << " received root Game Object of size " << go->getSize() << "\n";
		else if (go->getType() == REPLICA)
			EV << getName() << " " << getIndex() << " received replica Game Object of size " << go->getSize() << "\n";

		storage_msg->addObject(go);
		send(storage_msg, "write");
	}
	else if (pithos_m->getPayloadType() == INFORM)
	{
		super_peer_index = msg->getArrivalGate()->getIndex();
		EV << "A new super peer has been identified on gate " << super_peer_index << "\n";
	}
	else if (pithos_m->getPayloadType() == OVERLAY_WRITE)
	{
		EV << "An overlay write request was received, but this peer is not a super peer. The request will be ignored.\n";
	}
	else error ("Illegal P2P message received");
}

void Peer_logic::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "request") == 0)
	{
		handleRequest(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "in") == 0)
	{
		handleP2PMsg(msg);
	}
	else error("Illegal message received");

	delete(msg);
}

void Peer_logic::GroupStore(PithosMsg *write, GameObject *go)
{
	int i;
	simtime_t sendDelay;
	int group_replicas = par("replicas_g");
	int storage_node_index;
	GameObject *go_dup;
	PithosMsg *write_dup;

	for (i = 0 ; i < group_replicas ; i++)
	{
		go_dup = go->dup();
		write_dup = write->dup();

		if (i > 0) {
			go_dup->setType(REPLICA);
			write_dup->setName("replica_write");
		}

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
}

void Peer_logic::OverlayStore(PithosMsg *write, GameObject *go)
{
	int i;
	simtime_t sendDelay;
	int overlay_replicas = par("replicas_sp");

	if (super_peer_index == UNKNOWN)
	{
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		return;
	}
	else if (super_peer_index == THIS)
	{
		EV << "The message originated at the super peer. This should still be handled.\n";
		return;
	}

	write->setPayloadType(OVERLAY_WRITE);
	write->setValue(overlay_replicas);
	go->setType(OVERLAY);
	write->setName("overlay_write");
	write->addObject(go);


	if (gate("out", super_peer_index)->getTransmissionChannel()->isBusy())
	{
		sendDelay = gate("out", super_peer_index)->getTransmissionChannel()->getTransmissionFinishTime()-simTime();
		sendDelayed(write, sendDelay, "out", super_peer_index);
		emit(busySignal, 1);
	}
	else {
		send(write, "out", super_peer_index);
		emit(busySignal, 0);
	}
}

void Peer_logic::sendObjectForStore(int64_t o_size)
{
	PithosMsg *write;
	GameObject *go;

	//Create the packet that will house the game object
	write = new PithosMsg("write");
	write->setByteLength(o_size);
	write->setPayloadType(WRITE);

	go = new GameObject("GameObject");
	go->setSize(o_size);

	//Send the message to be stored on the specified number of replicas in the group.
	GroupStore(write, go);

	OverlayStore(write, go);	//Send the message to be stored on the specified number of replicas in the overlay.

}
