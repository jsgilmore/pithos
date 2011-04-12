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
	largestKey = par("largestKey");

	event = new cMessage("event");

	//Initialise queue statistics collection
	OverlayWriteSignal = registerSignal("OverlayWrite");

	scheduleAt(simTime(), event);
}

void Super_peer_logic::finish()
{

}

void Super_peer_logic::sp_identify()
{

	PithosMsg *inform = new PithosMsg("inform_req");
	inform->setPayloadType(INFORM_REQ);
	inform->setByteLength(sizeof(int));	//This is the size of the int sent in the packet

	send(inform, "peer_gate$o");
}

void Super_peer_logic::handleOverlayWrite(PithosMsg *pithos_m)
{
	if (pithos_m->getPayloadType() == OVERLAY_WRITE)
	{
		IPvXAddress *ip = new IPvXAddress("10.0.0.1");
		TransportAddress *address = new TransportAddress(*ip, 2048);

		for (int i = 0; i < pithos_m->getValue(); i++)
		{
			// let's create a random key
			OverlayKey randomKey(intuniform(1, largestKey));

			OverlayMsg *overlay_m = new OverlayMsg(); // the message we'll send
			overlay_m->setType(WRITE); // set the message type to PING
			overlay_m->setSenderAddress(*address); // set the sender address to our own
			overlay_m->addObject(pithos_m->removeObject("GameObject")); // set the message length to 100 bytes

			//EV << thisNode.getIp() << ": Sending packet to " << randomKey << "!" << std::endl;

			//callRoute(randomKey, overlay_m); // send it to the overlay
		}
	}
	else error("Expected OVERLAY_WRITE from local peers, but unknown packet received");

	emit(OverlayWriteSignal, 1);
}

void Super_peer_logic::handleMessage(cMessage *msg)
{
	if (msg == event)	//It's important that this is the first if, because there exists no arrival gate if the message is an event.
	{
		sp_identify();	//Broadcast the index of this super peer to all peers
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "peer_gate$i") == 0)
	{
		PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

		if (pithos_m->getPayloadType() == OVERLAY_WRITE)
		{
			handleOverlayWrite(pithos_m);
		}
		else error("Super peer received an unknown message");

	}
	else error("Unknown message received at Super peer logic");

	delete(msg);
}
