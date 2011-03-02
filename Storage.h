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

#ifndef PEER_H_
#define PEER_H_

#include <omnetpp.h>

class Storage : public cSimpleModule
{
	public:
		Storage();
		virtual ~Storage();
		int getStorageBytes();
		int getStorageFiles();
		int getObjectSize(int index);
		void storeObject(int o_size);
	private:
		int storage_size;
		int storage[];
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		void sendObjectForStore(int o_size);
};

Define_Module(Storage);

#endif /* PEER_H_ */
