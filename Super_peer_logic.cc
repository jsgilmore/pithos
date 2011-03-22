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
	network_size = par("network_size");

	//Initialise queue statistics collection
	busySignal = registerSignal("busy");
	emit(busySignal, 0);
}

void Super_peer_logic::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "request") == 0)
	{
		handleRequest(msg);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "in") == 0)
	{
		handleStore(msg);
	}
	else {
		EV << "Illegal message received\n";
	}

	delete(msg);
}
