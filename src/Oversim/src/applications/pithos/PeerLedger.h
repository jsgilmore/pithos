//
// Copyright (C) 2011 MIH Media lab, University of Stellenbosch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef PEERLEDGER_H_
#define PEERLEDGER_H_

#include <omnetpp.h>

#include "PeerData.h"
#include "Communicator.h"
#include "ObjectData.h"

class PeerLedger
{
	private:
		//Smart pointers are not required here, since the pointers do not point to elements in dynamic containers
		std::vector<ObjectDataPtr> objectDataList;

		unsigned int object_total;
		unsigned int times_recorded;

	public:

		PeerDataPtr peerDataPtr;

		PeerLedger();
		virtual ~PeerLedger();

		/**
		 * Add a TransportAddress to the list of object locations
		 *
		 * @param adr A TransportAddress containing the IP and port information
		 */
		void addObjectRef(ObjectDataPtr object_data_ptr);

		ObjectDataPtr getObjectRef(const int &i);

		unsigned int getObjectListSize();

		bool isObjectPresent(ObjectDataPtr object_ptr);

		void eraseObjectRef(const int &i);

		void eraseObjectRef(ObjectDataPtr object_data_ptr);

		//Record the number of objects to be able to calculate an average at the end of life. This should be done at regular intervals, otherwise the average is incorrect.
		void recordObjectNum();
		double getObjectAverage();
};

#endif /* PEERLEDGER_H_ */
