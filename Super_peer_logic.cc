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

	//Initialise queue statistics collection
	OverlayWriteSignal = registerSignal("OverlayWrite");
}

void Super_peer_logic::finish()
{

}

void Super_peer_logic::handleOverlayWrite(groupPkt *group_p)
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

void Super_peer_logic::handleBootstrapPkt(cMessage *msg)
{
	/*bootstrapPkt *boot_req = check_and_cast<bootstrapPkt *>(msg);

	EV << "Received bootstrap message from Node: " << boot_req->getSourceAddress() << endl;

	bootstrapPkt *boot_ans = new bootstrapPkt();
	boot_ans->setPayloadType(INFORM);
	boot_ans->setName("inform");
	boot_ans->setSourceAddress(boot_req->getDestinationAddress());
	boot_ans->setByteLength(4+4+4);	//Type, Src IP as # and Dest IP as #
	boot_ans->setDestinationAddress(boot_req->getSourceAddress());

	boot_ans->setSuperPeerAdr(findAddress(boot_req->getLatitude(), boot_req->getLongitude()));

	EV << "Directory server received an address request and returned " << boot_ans->getSuperPeerAdr() << " as a result\n";

	sendMessageToUDP(boot_ans->getDestinationAddress(), boot_ans);*/

	EV << "This section is not working yet. Well done for getting here!\n";

	//The original message is deleted in the calling function.
}

void Super_peer_logic::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		if (strcmp(msg->getClassName(), "groupPkt") == 0)
		{
			groupPkt *group_p = check_and_cast<groupPkt *>(msg);

			if (group_p->getPayloadType() == OVERLAY_WRITE_REQ)
			{
				handleOverlayWrite(group_p);
			}
			else {
				EV << "Message type: " << group_p->getPayloadType() << " name: " << group_p->getName() << endl;
				error("Super peer received an unknown message");
			}
		}
		else if (strcmp(msg->getClassName(), "bootstrapPkt") == 0)
		{
			handleBootstrapPkt(msg);
		}
	}
	else {
		char msg_str[100];
		sprintf(msg_str, "Unknown message received at Super peer logic (%s)", msg->getName());
		error(msg_str);
	}

	delete(msg);
}
