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

#include "Super_peer_logic.h"

Super_peer_logic::Super_peer_logic()
{
}

Super_peer_logic::~Super_peer_logic()
{

}

void Super_peer_logic::initialize()
{
	event = new cMessage("event");
	super_peer_index = THIS;

	network_size = par("network_size");

	//Initialise queue statistics collection
	busySignal = registerSignal("busy");
	OverlayWriteSignal = registerSignal("OverlayWrite");
	emit(busySignal, 0);

	scheduleAt(simTime(), event);
}

void Super_peer_logic::sp_identify()
{
	int i;
	PithosMsg *inform = new PithosMsg("inform");
	inform->setPayloadType(INFORM);
	inform->setByteLength(sizeof(int));	//This is the size of the int sent in the packet

	for (i = 0 ; i < network_size-2 ; i++)
	{
		send(inform->dup(), "out", i);
	}
	delete(inform);
}

void Super_peer_logic::handleOverlayWrite(PithosMsg *pithos_m)
{
	EV << "Overlay write request received at super peer. Congratualations! You've reached the end of what has thus far been implemented!\n";
	emit(OverlayWriteSignal, 1);
}

void Super_peer_logic::handleP2PMsg(cMessage *msg)
{
	PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

	if (pithos_m->getPayloadType() == OVERLAY_WRITE)
	{
		handleOverlayWrite(pithos_m);
	}
	else Peer_logic::handleP2PMsg(msg);
}

void Super_peer_logic::handleMessage(cMessage *msg)
{
	if (msg == event)	//It's important that this is the first if, because there exists no arrival gate if the message is an event.
	{
		sp_identify();	//Broadcast the index of this super peer to all peers
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "request") == 0)
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
