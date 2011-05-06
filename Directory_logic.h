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

#ifndef DIRECTORY_LOGIC_H_
#define DIRECTORY_LOGIC_H_

#include <omnetpp.h>
#include <iostream>
#include <vector>

#include "BaseApp.h"

#include "bootstrapPkt_m.h"
#include "groupPkt_m.h"

#include "SP_element.h"

class Directory_logic : public BaseApp
{
	private:
		std::vector<SP_element> sp_adr_list;			//The list that holds the combination of Super Peer IPs and their positions in the world

	protected:
		void initializeApp(int stage);                 // called when the module is being created
		void finishApp();                              // called when the module is about to be destroyed
		void handleTimerEvent(cMessage* msg);          // called when we received a timer message
		void deliver(OverlayKey& key, cMessage* msg);  // called when we receive a message from the overlay
		void handleUDPMessage(cMessage* msg);          // called when we receive a UDP message
		void handleBootstrapPkt(cMessage *msg);
		TransportAddress findAddress(double lati, double longi);

	public:
		Directory_logic();
		~Directory_logic();
};

Define_Module(Directory_logic);

#endif /* DIRECTORY_LOGIC_H_ */
