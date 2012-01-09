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

#include "OverlayStorage.h"

Define_Module(OverlayStorage);

OverlayStorage::OverlayStorage() {
	// TODO Auto-generated constructor stub

}

OverlayStorage::~OverlayStorage() {
	// TODO Auto-generated destructor stub
}

void OverlayStorage::initialize()
{

}

void OverlayStorage::store(GameObject *go)
{
	simtime_t sendDelay;
	GroupStorage * this_peer = ((GroupStorage *)getParentModule()->getSubmodule("group_storage"));
	int overlay_replicas = par("replicas");

	const NodeHandle *thisNode = &(((BaseApp *)getParentModule()->getSubmodule("communicator"))->getThisNode());
	TransportAddress sourceAdr(thisNode->getIp(), thisNode->getPort());

	ValuePkt *overlay_write = new ValuePkt();
	overlay_write->setByteLength(4+4+4+4+8+go->getSize());	//Source address, dest address, type, value, object name ID, object size

	if (!(this_peer->hasSuperPeer()))
	{
		//TODO: This error condition should be logged
		EV << "No super peer has been identified. The object will not be replicated in the Overlay\n";
		delete(overlay_write);
		delete(go);
		return;
	}

	go->setType(OVERLAY);

	overlay_write->setPayloadType(OVERLAY_WRITE_REQ);
	overlay_write->setSourceAddress(sourceAdr);
	overlay_write->setValue(overlay_replicas);
	overlay_write->setName("overlay_write_req");
	overlay_write->setDestinationAddress(this_peer->getSuperPeerAddress());
	overlay_write->addObject(go);

	send(overlay_write, "comms_gate$o");		//Set address
}

void OverlayStorage::handleMessage(cMessage *msg)
{
	GameObject *go = (GameObject *)msg->removeObject("GameObject");

	if (go == NULL)
		error("No object was attached to be stored in group storage");

	store(go);

	delete(msg);
}
