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

#ifndef OBJECTLEDGER_H_
#define OBJECTLEDGER_H_

#include <omnetpp.h>
#include <string>
#include <vector>


#include "PeerData.h"
#include "ObjectData.h"

/**
 * This class stores the information of a single object, including name,
 * size and a list of peers that store the object. The class is used by
 * super peers as part of a vector to keep track of which game objects
 * reside where in the group.
 *
 * @author John Gilmore
 */
class ObjectLedger
{
	private:

		/**
		 * A list of peers this object is stored in, in the group.
		 * A shared_ptr smart pointer is used, because this list is a list of pointers to the vector that stored the actual peer information. Pointers are used,
		 * because multiple objects will contain sets of overlapping peers, which would create duplicate information.
		 * Smart pointers are used, because a std::vector's memory addresses may change when more memory is required for an insert.
		 * Smart pointers ensure that the memory region which is pointed too will always be available.
		 */
		std::vector<PeerDataPtr> location_list;

		//Records the number of times an object has been stored (object removals do not subtract from this number)
		int replications;
		int repairs;

	public:

		ObjectDataPtr objectDataPtr;

		ObjectLedger();
		virtual ~ObjectLedger();

		/**
		 * Add a peer data item to the list of object locations
		 *
		 * @param peer_data_ptr A smart pointer to a peer data object, containing the peer's transport address
		 */
		void addPeerRef(PeerDataPtr peer_data_ptr);

		PeerDataPtr getPeerRef(const int &i);

		/**
		 * @returns a random address from the list of object locations
		 */
		PeerDataPtr getRandPeerRef();

		unsigned int getPeerListSize();

		bool isPeerPresent(PeerData peer_data);

		void erasePeerRef(const int &i);

		void erasePeerRef(PeerDataPtr peer_data_ptr);

		void resetReplications();
		int getReplications();
		void resetRepairs();
		int getRepairs();
		void addRepairs(int adds);
};

#endif /* OBJECTLEDGER_H_ */
