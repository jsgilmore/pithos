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

#ifndef OVERLAYSTORAGE_H_
#define OVERLAYSTORAGE_H_

#include <omnetpp.h>

#include "BaseApp.h"
#include "Peer_logic.h"

#include "groupPkt_m.h"
#include "GameObject.h"

class OverlayStorage : public cSimpleModule
{
	public:
		OverlayStorage();
		virtual ~OverlayStorage();
	private:
		void store(GameObject *go);
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
};

Define_Module(OverlayStorage);

#endif /* OVERLAYSTORAGE_H_ */
