//
// Copyright (C) 2009 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @author Antonio Zea
 */

#include <string>

#include "Communicator.h"

Define_Module(Communicator);

// initializeApp() is called when the module is being created.
// Use this function instead of the constructor for initializing variables.
void Communicator::initializeApp(int stage)
{
    // initializeApp will be called twice, each with a different stage.
    // stage can be either MIN_STAGE_APP (this module is being created),
    // or MAX_STAGE_APP (all modules were created).
    // We only care about MIN_STAGE_APP here.

    if (stage != MIN_STAGE_APP) return;

    // copy the module parameter values to our own variables
    largestKey = par("largestKey");

    // initialize our statistics variables
    numSent = 0;
    numReceived = 0;

    // tell the GUI to display our variables
    WATCH(numSent);
    WATCH(numReceived);

    // start our timer!
    timerMsg = new cMessage("MyApplication Timer");

    bindToPort(2000);
}


// finish is called when the module is being destroyed
void Communicator::finishApp()
{
    // finish() is usually used to save the module's statistics.
    // We'll use globalStatistics->addStdDev(), which will calculate min, max, mean and deviation values.
    // The first parameter is a name for the value, you can use any name you like (use a name you can find quickly!).
    // In the end, the simulator will mix together all values, from all nodes, with the same name.

    globalStatistics->addStdDev("MyApplication: Sent packets", numSent);
    globalStatistics->addStdDev("MyApplication: Received packets", numReceived);
}


// handleTimerEvent is called when a timer event triggers
void Communicator::handleTimerEvent(cMessage* msg)
{

}

// deliver() is called when we receive a message from the overlay.
// Unknown packets can be safely deleted here.
void Communicator::deliver(OverlayKey& key, cMessage* msg)
{
    // we are only expecting messages of type MyMessage, throw away any other
	//CommunicatorMsg *myMsg = dynamic_cast<CommunicatorMsg*>(msg);

	send(msg, "sp_gate$o");
}

// handleUDPMessage() is called when we receive a message from UDP.
// Unknown packets can be safely deleted here.
void Communicator::handleUDPMessage(cMessage* msg)
{
	PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

	if (pithos_m->getPayloadType() == OVERLAY_WRITE_REQ)
	{
		if (strcmp(getParentModule()->getName(), "Super_peer") == 0)
			send(msg, "sp_gate$o");
		else EV << "An overlay write request was received, but this peer is not a super peer. The request will be ignored.\n";
	}
	else send(msg, "peer_gate$o");
}

void Communicator::handleSPMsg(cMessage *msg)
{
	int i;
	PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);
	int network_size = GlobalNodeListAccess().get()->getNumNodes();

	PithosMsg *pithos_dup;

	char ip[16];
	IPvXAddress dest_ip;
	TransportAddress dest_adr;

	if (pithos_m->getPayloadType() == INFORM_REQ)
	{
		pithos_m->setPayloadType(INFORM);
		pithos_m->setName("inform");
		pithos_m->setSourceAddress(TransportAddress(thisNode.getIp(), thisNode.getPort()));


		for (i = 0 ; i <  network_size ; i++)
		{
			pithos_dup = pithos_m->dup();
			//Set the dest IP dynamically
			if (i>255)
				error("IP exceeds network range");
			sprintf(ip, "1.0.0.%d", i+1);
			dest_ip.set(ip);
			dest_adr.setIp(dest_ip, 2000);

			pithos_dup->setDestinationAddress(dest_adr);	//FIXME: Add address sending

			sendMessageToUDP(dest_adr, pithos_dup);
		}
		//The original message is deleted in the handMessage function.
	}
	else error("Peer logic received invalid packet from super peer logic");
}

void Communicator::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "sp_gate$i") == 0)
	{
		handleSPMsg(msg);
		delete(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "peer_gate$i") == 0)
	{
		PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

		if (pithos_m->getPayloadType() == OVERLAY_WRITE_REQ)
		{
			if (strcmp(getParentModule()->getName(), "Super_peer") == 0)
			{
				send(msg, "sp_gate$o");
			} else {
				sendMessageToUDP(pithos_m->getDestinationAddress(), pithos_m);	//The super peer logic should have added the super peer's dest address here
			}
		}
		else if (pithos_m->getPayloadType() == WRITE)
		{
			sendMessageToUDP(pithos_m->getDestinationAddress(), pithos_m);
		}
		else error("Unknown message received from super_peer_logic");

	} else BaseApp::handleMessage(msg);
}
