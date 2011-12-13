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

Define_Module(Storage);

Storage::Storage()
{
}

Storage::~Storage()
{
}

void Storage::initialize()
{
	storage.setName("queue");
	take(&storage);

	//Initialise queue statistics collection
	qlenSignal = registerSignal("qlen");
	qsizeSignal = registerSignal("qsize");

	storeTimeSignal = registerSignal("storeTime");
	rootStoreTimeSignal = registerSignal("rootStoreTime");
	replicaStoreTimeSignal = registerSignal("replicaStoreTime");
	overlayStoreTimeSignal = registerSignal("overlayStoreTime");

	emit(qlenSignal, storage.length());
	emit(qsizeSignal, getStorageBytes());

	overlayObjectsSignal = registerSignal("OverlayObject");
	rootObjectsSignal = registerSignal("RootObject");
	replicaObjectsSignal = registerSignal("ReplicaObject");
}

int Storage::getStorageBytes()
{
	int i;
	int total_size = 0;

	//This is inefficient, since a sequential search will be done for every element in the queue.
	//TODO: The "forEachChild" method should rather be implemented with an appropriate visitor class.
	for (i = 0 ; i < storage.getLength() ; i++)
	{
		total_size += ((GameObject *)storage.get(i))->getSize();
	}

	return total_size;
}

int Storage::getStorageFiles()
{
	return storage.getLength();
}

void Storage::handleMessage(cMessage *msg)
{
	simtime_t delay;
	if (!(msg->hasObject("GameObject")))
		error("Storage received a message with no game object attached");

	GameObject *go = (GameObject *)msg->removeObject("GameObject");

	if (go->getType() == ROOT)
		EV << getName() << " " << getIndex() << " received root Game Object of size " << go->getSize() << "\n";
	else if (go->getType() == REPLICA)
		EV << getName() << " " << getIndex() << " received replica Game Object of size " << go->getSize() << "\n";

	delay = simTime() - go->getCreationTime();

	EV << getName() << " " << getIndex() << " received write command of size " << go->getSize() << " with delay " << go->getCreationTime() << "\n";

	emit(storeTimeSignal, delay);

	storage.insert(go);

	emit(qlenSignal, storage.length());
	emit(qsizeSignal, getStorageBytes());

	if (go->getType() == ROOT)
	{
		emit(rootObjectsSignal, 1);
		emit(rootStoreTimeSignal, delay);
	}
	else if (go->getType() == REPLICA)
	{
		emit(replicaObjectsSignal, 1);
		emit(replicaStoreTimeSignal, delay);
	}
	else if (go->getType() == OVERLAY)
	{
		emit(overlayObjectsSignal, 1);
		emit(overlayStoreTimeSignal, delay);
	}
	else error("The game object type was incorrectly set");

	delete(msg);
}
