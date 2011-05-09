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
	//All messages received from the overlay, should be sent to the super peer
    send(msg, "sp_gate$o");
}

// handleUDPMessage() is called when we receive a message from UDP.
// Unknown packets can be safely deleted here.
void Communicator::handleUDPMessage(cMessage* msg)
{

	numReceived++;

	if (strcmp(msg->getClassName(), "groupPkt") == 0)
	{
		groupPkt *group_p = check_and_cast<groupPkt *>(msg);

		if (group_p->getPayloadType() == OVERLAY_WRITE_REQ)
		{
			if (getParentModule()->getSubmodule("super_peer_logic") != NULL)
				send(msg, "sp_gate$o");
			else {
				EV << "An overlay write request was received, but this peer (" << getParentModule()->getName() << ") is not a super peer. The request will be ignored.\n";
				delete(msg);
			}
		}
		else {
			EV << "Received group packet of type " << group_p->getPayloadType() << endl;
			send(msg, "peer_gate$o");
		}
	}
	else if (strcmp(msg->getClassName(), "bootstrapPkt") == 0)
	{
		bootstrapPkt *boot_p = check_and_cast<bootstrapPkt *>(msg);

		if (boot_p->getPayloadType() == JOIN_REQ)
		{
			if (getParentModule()->getSubmodule("super_peer_logic") != NULL)
				send(msg, "sp_gate$o");
			else {
				EV << "A super peer join request was received, but this peer (" << getParentModule()->getName() << ": " << getParentModule()->getIndex() << ") is not a super peer. The request will be ignored.\n";
				delete(msg);
			}
		} else send(msg, "peer_gate$o");
	}
	else error("Communicator received unknown message from UDP");
}

void Communicator::handleSPMsg(cMessage *msg)
{
	/*int i;
	groupPkt *group_p = check_and_cast<groupPkt *>(msg);
	int network_size = GlobalNodeListAccess().get()->getNumNodes();

	groupPkt *group_p_dup;

	char ip[16];
	IPvXAddress dest_ip;
	TransportAddress dest_adr;

	if (group_p->getPayloadType() == INFORM_REQ)
	{
		group_p->setPayloadType(INFORM);
		group_p->setName("inform");
		group_p->setSourceAddress(TransportAddress(thisNode.getIp(), thisNode.getPort()));
		group_p->setByteLength(4+4+4);	//Type, Src IP as # and Dest IP as #


		for (i = 0 ; i <  network_size ; i++)
		{
			group_p_dup = group_p->dup();
			//Set the dest IP dynamically
			if (i>255)
				error("IP exceeds network range");
			sprintf(ip, "1.0.0.%d", i+2);		//The IP address should start at 1, but also ignore the first IP, which is the directory server
			dest_ip.set(ip);
			dest_adr.setIp(dest_ip, 2000);

			group_p_dup->setDestinationAddress(dest_adr);	//FIXME: Add address sending

			sendMessageToUDP(dest_adr, group_p_dup);

			numSent++;
		}
		//The original message is deleted in the handMessage function.
	}
	else error("Peer logic received invalid packet from super peer logic");*/

	error("The communicator is not expecting any message from the Super Peer logic at this time.");		//TODO: Remove this error when the Super peer starts to send again
	delete(msg);
}

void Communicator::handlePeerMsg(cMessage *msg)
{
	Packet *pkt = check_and_cast<Packet *>(msg);

	sendMessageToUDP(pkt->getDestinationAddress(), pkt);

	numSent++;
}

void Communicator::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "sp_gate$i") == 0)
	{
		handleSPMsg(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "peer_gate$i") == 0)
	{
		handlePeerMsg(msg);
	} else BaseApp::handleMessage(msg);
}
