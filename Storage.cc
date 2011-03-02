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
#include "Pithos_m.h"

Storage::Storage()
{
	storage_size = 0;
	WATCH(storage_size);
	WATCH(storage);
}

Storage::~Storage() {
	// TODO Auto-generated destructor stub
}

void Storage::initialize()
{

}

void Storage::handleMessage(cMessage *msg)
{
	PithosMsg *pithos_m = check_and_cast<PithosMsg *>(msg);

	if (pithos_m->getPayloadType() == STORE_REQ)
	{
		EV << getParentModule()->getName() << " " << getParentModule()->getIndex() << " received store request of size " << pithos_m->getByteLength() << "\n";

		sendObjectForStore(pithos_m->getByteLength());

		delete(pithos_m);
	}
	else if (pithos_m->getPayloadType() == WRITE)
	{
		EV << getName() << " " << getIndex() << " received write command of size " << pithos_m->getByteLength() << "\n";

		storeObject(pithos_m->getByteLength());
		delete(pithos_m);
	}
	else {
		EV << "Illegal message received\n";
		delete(pithos_m);
	}
}

int Storage::getStorageBytes()
{
	int i;
	int total_size = 0;

	for (i = 0 ; i < storage_size ; i++)
	{
		total_size += storage[i];
	}

	return total_size;
}

int Storage::getStorageFiles()
{
	return storage_size;
}

int Storage::getObjectSize(int index)
{
	return storage[index];
}

void Storage::sendObjectForStore(int o_size)
{
	int storage_node_index = intuniform(0, 19);

	PithosMsg *write = new PithosMsg("write");
	write->setByteLength(o_size);
	write->setPayloadType(WRITE);

	send(write, "out", storage_node_index);
}

void Storage::storeObject(int o_size)
{
	storage[storage_size++] = o_size;
}
