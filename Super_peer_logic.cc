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
	cancelAndDelete(event);
}

void Super_peer_logic::initialize()
{
	largestKey = par("largestKey");

	event = new cMessage("event");

	//Initialise queue statistics collection
	OverlayWriteSignal = registerSignal("OverlayWrite");

	scheduleAt(simTime()+par("wait_time"), event);
}

void Super_peer_logic::finish()
{

}

void Super_peer_logic::sp_identify()
{

	PithosMsg *inform = new PithosMsg("inform_req");
	inform->setPayloadType(INFORM_REQ);
	inform->setByteLength(sizeof(int));	//This is the size of the int sent in the packet

	send(inform, "comms_gate$o");
}

void Super_peer_logic::handleOverlayWrite(PithosMsg *pithos_m)
{
	/*GameObject *go = (GameObject *)pithos_m->removeObject("GameObject");

	for (int i = 0; i < pithos_m->getValue(); i++)
	{
		// let's create a random key
		OverlayKey randomKey(intuniform(1, largestKey));

		OverlayMsg *overlay_m = new OverlayMsg(); // the message we'll send
		overlay_m->setType(WRITE); // set the message type to PING
		overlay_m->addObject(go->dup());

		EV << ((BaseApp *)getParentModule()->getSubmodule("Communicator"))->getThisNode().getIp() << ": Sending packet to " << randomKey << "!" << std::endl;


		callRoute(randomKey, overlay_m); // send it to the overlay
	}
	delete(go);

	emit(OverlayWriteSignal, 1);*/

	EV << "Packet reached Super peer. That's all folks\n";
}

void Super_peer_logic::handleMessage(cMessage *msg)
{
	if (msg == event)	//It's important that this is the first if, because there exists no arrival gate if the message is an event.
	{
		sp_identify();	//Broadcast the index of this super peer to all peers
		scheduleAt(simTime()+par("wait_time"), event);	//Reschedule the message to be broadcast to all peers, so that new peers might also learn of the super peer.
		return;
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

		if (pithos_m->getPayloadType() == OVERLAY_WRITE_REQ)
		{
			handleOverlayWrite(pithos_m);
		}
		else {
			EV << "Message type: " << pithos_m->getPayloadType() << " name: " << pithos_m->getName() << endl;
			error("Super peer received an unknown message");
		}

	}
	else {
		char msg_str[100];
		sprintf(msg_str, "Unknown message received at Super peer logic (%s)", msg->getName());
		error(msg_str);
	}

	delete(msg);
}
