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

#include "PeerData.h"
#include "ObjectData.h"

class PeerLedger
{
	private:
		//Smart pointers are not required here, since the pointers do not point to elements in dynamic containers
		std::vector<ObjectDataPtr> objectDataList;

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

		int getObjectListSize();

		bool isObjectPresent(ObjectDataPtr object_ptr);
};

#endif /* PEERLEDGER_H_ */
