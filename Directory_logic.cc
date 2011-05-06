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

    SP_element element;
    element.setAddress("1.0.0.2", 2000);
    element.setPosition(0.0, 0.0);
    sp_adr_list.reserve(20);	//The amount of memory to initially reserve for this vector
    sp_adr_list.push_back(element);

    EV << "Initialising directory server with a super peer at address " << element.getAddress().getIp() << endl;

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
		error("No super peer found to match IP");

	return ((SP_element)sp_adr_list.at(place)).getAddress();
}

void Directory_logic::handleBootstrapPkt(cMessage *msg)
{
	bootstrapPkt *boot_req = check_and_cast<bootstrapPkt *>(msg);

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

// handleUDPMessage() is called when we receive a message from UDP.
// Unknown packets can be safely deleted here.
void Directory_logic::handleUDPMessage(cMessage* msg)
{
	if (strcmp(msg->getClassName(), "bootstrapPkt") == 0)
	{
		handleBootstrapPkt(msg);
	}
	else EV << "The directory server received an unknown message type, ignoring\n";

	delete(msg);
}
