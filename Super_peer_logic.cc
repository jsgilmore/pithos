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
	GameObject *go = (GameObject *)group_p->removeObject("GameObject");
	if (go == NULL)
		error("No game object found attached to the message\n");

	//FIXME: The hash string should still be adapted to allow for multiple replicas in the overlay
	for (int i = 0; i < group_p->getValue(); i++)
	{
		OverlayPkt *overlay_p = new OverlayPkt(); // the message we'll send
		overlay_p->setType(WRITE); // set the message type to PING
		overlay_p->setByteLength((8+4) + 4 + 20);	//Game object size and type + packet type + routing key
		overlay_p->setName("overlay_write");

		overlay_p->addObject(go->dup());

		send(overlay_p, "comms_gate$o"); // send it to the overlay
	}
	delete(go);

	emit(OverlayWriteSignal, 1);

	EV << "Packet sent for storage in the overlay\n";
}

void Super_peer_logic::handleBootstrapPkt(cMessage *msg)
{
	unsigned int i;
	bootstrapPkt *boot_req = check_and_cast<bootstrapPkt *>(msg);
	NodeHandle thisNode = ((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode();
	TransportAddress *sourceAdr = new TransportAddress(thisNode.getIp(), thisNode.getPort());

	EV << "Super peer received bootstrap request from " << boot_req->getSourceAddress() << ", sending list and updating group.\n";

	//IP data entry for the requesting peer to be added to the group peers list.
	PeerData peer_dat;

	//List packet that will be returned to the requesting peer
	PeerListPkt *list_p = new PeerListPkt();

	//Set the values of the packet to be returned to the requesting peer.
	//Set the type and byte length
	list_p->setName("join_accept");
	list_p->setPayloadType(JOIN_ACCEPT);
	list_p->setByteLength(2*sizeof(int)+sizeof(int)*2*group_peers.size());	//Value+Type+(IP+Port)*list_length FIXME: The size needs to still be multiplied by the size of the peer list.
	list_p->setSourceAddress(*sourceAdr);
	list_p->setDestinationAddress(boot_req->getSourceAddress());

	for (i = 0 ; i < group_peers.size() ; i++)
	{
		list_p->addToPeerList(group_peers.at(i));
	}
	send(list_p->dup(), "comms_gate$o");	//Send a copy of the peer list, so the original packet may be reused to inform the other nodes

	peer_dat.setAddress(boot_req->getSourceAddress());
	list_p->clearPeerList();	//This erases all data added to the peer list.
	list_p->addToPeerList(peer_dat);
	list_p->setByteLength(2*sizeof(int)+sizeof(int)*2);	//Value+Type+(IP+Port)

	for (i = 0 ; i < group_peers.size() ; i++)
	{
		list_p->setDestinationAddress(((PeerData)group_peers.at(i)).getAddress());

		send(list_p->dup(), "comms_gate$o");
	}
	delete(list_p);

	//Add the data of the requesting peer into the list.
	group_peers.push_back(peer_dat);

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
