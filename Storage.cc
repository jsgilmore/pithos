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

#include "Storage.h"

Storage::Storage()
{
}

Storage::~Storage() {
	delete(storage);
}

void Storage::initialize()
{
	storage = new cQueue();
	storage->setName("queue");

	//Initialise queue statistics collection
	qlenSignal = registerSignal("qlen");
	busySignal = registerSignal("busy");
	queueingTimeSignal = registerSignal("queueingTime");
	emit(qlenSignal, storage->length());
	emit(busySignal, 0);
}

void Storage::handleMessage(cMessage *msg)
{
	if (strcmp(msg->getArrivalGate()->getName(), "write") == 0)
	{
		Message *m = check_and_cast<Message *>(msg);
		EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received store request of size " << m->getValue() << "\n";

		sendObjectForStore(m->getValue());

		delete(m);
	}
	else if (strcmp(msg->getArrivalGate()->getName(), "in") == 0)
	{
		PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

		EV << getName() << " " << getIndex() << " received write command of size " << pithos_m->getByteLength() << "\n";
		storeObject(pithos_m->getByteLength());
		delete(pithos_m);
	}
	else {
		EV << "Illegal message received\n";
		delete(msg);
	}
}

int Storage::getStorageBytes()
{
	int i;
	int total_size = 0;

	//This is inefficient, since a sequential search will be done for every element in the queue.
	//TODO: The "forEachChild" method should rather be implemented with an appropriate visitor class.
	for (i = 0 ; i < storage->getLength() ; i++)
	{
		total_size += ((go *)storage->get(i))->getSize();
	}

	return total_size;
}

int Storage::getStorageFiles()
{
	return storage->getLength();
}

void Storage::sendObjectForStore(int64_t o_size)
{
	int storage_node_index = intuniform(0, 18);	//18. because there are 20 nodes, which means 19 connections, which means 0-18 values.
	simtime_t sendDelay = gate("out", storage_node_index)->getTransmissionChannel()->getTransmissionFinishTime()-simTime();

	PithosMsg *write = new PithosMsg("write");
	write->setByteLength(o_size);
	write->setPayloadType(WRITE);

	if (gate("out", storage_node_index)->getTransmissionChannel()->isBusy())
	{
		sendDelayed(write, sendDelay, "out", storage_node_index);
		emit(busySignal, 1);
	}
	else {
		send(write, "out", storage_node_index);
		emit(busySignal, 0);
	}
}

void Storage::storeObject(int64_t o_size)
{
	go *game_object = new go();
	game_object->setSize(o_size);
	storage->insert(game_object);

	emit(qlenSignal, storage->length());
}
