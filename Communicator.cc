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
	EV << "Message received over overlay!!!!!!!!!!!!!!!! (" << msg->getName() << ")\n";
	//All messages received from the overlay, should be sent to the super peer
    //send(msg, "sp_gate$o");
	delete(msg);
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
	else if (strcmp(msg->getClassName(), "PeerListPkt") == 0)
	{
		send(msg, "peer_gate$o");
	}
	else error("Communicator received unknown message from UDP");
}

void Communicator::handleSPMsg(cMessage *msg)
{
	if (underlayConfigurator->isInInitPhase())
	{
		delete(msg);
		error("Underlay configurator is still in init phase, extend wait time.\n");
	}

	if (strcmp(msg->getName(), "join_accept") == 0)
	{
		Packet *pkt = check_and_cast<Packet *>(msg);

		sendMessageToUDP(pkt->getDestinationAddress(), pkt);

		numSent++;
	}
	else if (strcmp(msg->getName(), "overlay_write") == 0)
	{
		CSHA1 hash;
		char hash_str[100];		//SHA-1 produces a 160 bit/20 byte hash

		GameObject *go = (GameObject *)msg->getObject("GameObject");
		cPacket *pkt = check_and_cast<cPacket *>(msg);

		//Create a hash of the game object's name
		hash.Update((unsigned char *)go->getObjectName(), strlen(go->getObjectName()));
		hash.Final();
		hash.ReportHash(hash_str, CSHA1::REPORT_HEX);

		OverlayKey nameKey(hash_str, 16);

		EV << thisNode.getIp() << ": Sending packet to " << nameKey << "!" << std::endl;

		callRoute(nameKey, pkt);
	}
}

void Communicator::handlePeerMsg(cMessage *msg)
{
	Packet *pkt = check_and_cast<Packet *>(msg);

	if (underlayConfigurator->isInInitPhase())
	{
		delete(msg);
		error("Underlay configurator is still in init phase, extend wait time.\n");
	}

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
