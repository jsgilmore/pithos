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

#ifndef PEER_LOGIC_H_
#define PEER_LOGIC_H_

#include <omnetpp.h>

#include "Pithos_m.h"
#include "Message_m.h"
#include "go.h"

class Peer_logic: public cSimpleModule
{
	public:
		Peer_logic();
		virtual ~Peer_logic();
	protected:
		simsignal_t busySignal;
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		void sendObjectForStore(int64_t o_size);
};

Define_Module(Peer_logic);

#endif /* PEER_LOGIC_H_ */
