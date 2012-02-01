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

#include "PeerLedger.h"

PeerLedger::PeerLedger() {
	// TODO Auto-generated constructor stub

}

PeerLedger::~PeerLedger() {
	// TODO Auto-generated destructor stub
}

void PeerLedger::addObjectRef(ObjectDataPtr object_data_ptr)
{
	ObjectDataPtr ptr(object_data_ptr);
	objectDataList.push_back(ptr);
}

ObjectDataPtr PeerLedger::getObjectRef(const int &i)
{
	return objectDataList.at(i);
}

bool PeerLedger::isObjectPresent(ObjectDataPtr object_ptr)
{
	std::vector<ObjectDataPtr>::iterator list_ptr;

	for (list_ptr = objectDataList.begin() ; list_ptr != objectDataList.end() ; list_ptr++)
	{
		//This checks whether the PeerData object have the same values
		if (**list_ptr == *object_ptr)
			return true;
	}

	return false;
}

int PeerLedger::getObjectListSize()
{
	return objectDataList.size();
}
