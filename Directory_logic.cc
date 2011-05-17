//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Directory_logic.h"

Directory_logic::Directory_logic() {


}

Directory_logic::~Directory_logic() {

}

// initializeApp() is called when the module is being created.
// Use this function instead of the constructor for initializing variables.
void Directory_logic::initializeApp(int stage)
{
    // initializeApp will be called twice, each with a different stage.
    // stage can be either MIN_STAGE_APP (this module is being created),
    // or MAX_STAGE_APP (all modules were created).
    // We only care about MIN_STAGE_APP here.

    if (stage != MIN_STAGE_APP) return;

    superPeerNumSignal = registerSignal("SuperPeerNum");

    sp_adr_list.reserve(20);	//The amount of memory to initially reserve for this vector

    bindToPort(2000);
}

// finish is called when the module is being destroyed
void Directory_logic::finishApp()
{
    // finish() is usually used to save the module's statistics.
}

// handleTimerEvent is called when a timer event triggers
void Directory_logic::handleTimerEvent(cMessage* msg)
{
	delete(msg);
}

// deliver() is called when we receive a message from the overlay.
// Unknown packets can be safely deleted here.
void Directory_logic::deliver(OverlayKey& key, cMessage* msg)
{
	delete(msg);
}

bool Directory_logic::superPeersExist()
{
	if (sp_adr_list.size() > 0) return true;
	else return false;
}

TransportAddress Directory_logic::findAddress(double lati, double longi)
{
	unsigned int i;
	double nearest_dist = 10000;	//TODO: Use a parameter that specifies the maximum distance in a given virtual world
	int place = -1;

	double list_lat;
	double list_long;
	double dist;

	for (i = 0 ; i < sp_adr_list.size() ; i++)
	{
		list_lat = ((SP_element)sp_adr_list.at(i)).getLattitude();
		list_long = ((SP_element)sp_adr_list.at(i)).getLongitude();

		dist = sqrt( pow(list_lat - lati, 2) + pow(list_long - longi, 2));

		if (dist < nearest_dist)
		{
			nearest_dist = dist;
			place = i;
		}

	}

	if (place == -1)
		error("No super peers present. Check first next time");

	return ((SP_element)sp_adr_list.at(place)).getAddress();
}

void Directory_logic::handleJoinReq(bootstrapPkt *boot_req)
{
	EV << "Received bootstrap message from Node: " << boot_req->getSourceAddress() << endl;

	bootstrapPkt *boot_ans = new bootstrapPkt();
	boot_ans->setPayloadType(INFORM);
	boot_ans->setName("inform");
	boot_ans->setSourceAddress(boot_req->getDestinationAddress());
	boot_ans->setByteLength(4+4+4);	//Type, Src IP as # and Dest IP as #
	boot_ans->setDestinationAddress(boot_req->getSourceAddress());

	boot_ans->setSuperPeerAdr(findAddress(boot_req->getLatitude(), boot_req->getLongitude()));

	EV << "Directory server received an address request and returned " << boot_ans->getSuperPeerAdr() << " as a result\n";

	sendMessageToUDP(boot_ans->getDestinationAddress(), boot_ans);

	//The original message is deleted in the calling function.
}

void Directory_logic::handleSuperPeerAdd(bootstrapPkt *boot_req)
{
	SP_element super_peer;

	super_peer.setAddress(boot_req->getSourceAddress());
	super_peer.setPosition(boot_req->getLatitude(), boot_req->getLongitude());

	sp_adr_list.push_back(super_peer);

	emit(superPeerNumSignal, 1);

	EV << "Received super peer information from Node: " << boot_req->getSourceAddress() << endl;

	//The original message is deleted in the calling function.
}

// handleUDPMessage() is called when we receive a message from UDP.
// Unknown packets can be safely deleted here.
void Directory_logic::handleUDPMessage(cMessage* msg)
{
	bootstrapPkt *boot_req = check_and_cast<bootstrapPkt *>(msg);

	if (boot_req->getPayloadType() == JOIN_REQ)
	{
		if (superPeersExist())
			handleJoinReq(boot_req);
	}
	else if (boot_req->getPayloadType() == SUPER_PEER_ADD)
	{
		handleSuperPeerAdd(boot_req);
	}
	else EV << "The directory server received an unknown message type, ignoring\n";

	delete(msg);
}
