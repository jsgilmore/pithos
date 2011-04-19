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
		super_peer_address = pithos_m->getSourceAddress();
		EV << "A new super peer has been identified" << super_peer_address.getIp() << "\n";
	}
	else error ("Illegal P2P message received");
}

void Peer_logic::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "request") == 0)
	{
		handleRequest(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "comms_gate$i") == 0)
	{
		handleP2PMsg(msg);
	}
	else error("Illegal message received");

	delete(msg);
}

void Peer_logic::GroupStore(PithosMsg *write, GameObject *go)
{
	int i, j;
	simtime_t sendDelay;
	int group_replicas = par("replicas_g");
	GameObject *go_dup;
	PithosMsg *write_dup;

	bool original_address;
	TransportAddress *send_list = new TransportAddress[group_replicas];

	int network_size = GlobalNodeListAccess().get()->getNumNodes();

	char ip[16];
	TransportAddress dest_adr;
	IPvXAddress dest_ip;
	int dest_port = 2000;	//TODO: Allow the user to change this port number
	dest_adr.setPort(dest_port);

	for (i = 0 ; i < group_replicas ; i++)
	{
		//FIXME: Edit this to handle IP overflows in the next sections
		if (i>255)
			error("IP exceeds network range");

		//Duplicates the objects for sending
		go_dup = go->dup();
		write_dup = write->dup();

		original_address = false;

		//FIXME: Add a timeout parameter, when there are too few known nodes
		while(!original_address)
		{
			//Set the dest IP dynamically
			//TODO: Change this to use a list of IP addresses available on the network and connected to the storage
			sprintf(ip, "1.0.0.%d", intuniform(0, network_size)+1);
			dest_ip.set(ip);
			dest_adr.setIp(dest_ip);

			//Check all previous chosen addresses to determine whether this address is unique
			original_address = true;
			for (j = 0 ; j < i ; j++)
			{
				if (send_list[j] == dest_adr)
					original_address = false;
			}
		}

		if (i > 0) {
			go_dup->setType(REPLICA);
			write_dup->setName("replica_write");
		}

		write_dup->addObject(go_dup);
		write_dup->setDestinationAddress(dest_adr);

		send_list[i].setIp(dest_ip, dest_port);

		send(write_dup, "comms_gate$o");		//Set sender address
		emit(busySignal, 0);
	}

	delete[]send_list;
}

void Peer_logic::OverlayStore(PithosMsg *write, GameObject *go)
{
	simtime_t sendDelay;
	int overlay_replicas = par("replicas_sp");

	if (super_peer_address.isUnspecified())
	{
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		delete(write);
		delete(go);
		return;
	}

	go->setType(OVERLAY);

	write->setPayloadType(OVERLAY_WRITE_REQ);
	write->setValue(overlay_replicas);
	write->setName("overlay_write");
	write->setDestinationAddress(super_peer_address);
	write->addObject(go);

	send(write, "comms_gate$o");		//Set address
	emit(busySignal, 0);
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
