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

#ifndef OBJECTINFO_H_
#define OBJECTINFO_H_

#include <string>
#include <vector>
#include <tr1/memory>

#include "PeerData.h"

/**
 * PeerDataPtr is a typedef for the shared_ptr smart pointer type, which using
 * reference counting to ensure that memory is never freed while there are still
 * pointers pointing to it. It also automatically frees memory if no pointers are
 * pointing to it anymore. Their use is requried when creating pointers to dynamic
 * structures like vectors.
 */
typedef std::tr1::shared_ptr <PeerData> PeerDataPtr;

/**
 * This class stores the information of a single object, including name,
 * size and a list of peers that store the object. The class is used by
 * super peers as part of a vector to keep track of which game objects
 * reside where in the group.
 *
 * @author John Gilmore
 */
class ObjectInfo
{
	private:

		/** Object name */
		std::string object_name;

		/** Object size in bytes */
		int size;

		/**
		 * A list of peers this object is stored in, in the group.
		 * A shared_ptr smart pointer is used, because this list is a list of pointers to the vector that stored the actual peer information. Pointers are used,
		 * because multiple objects will contain sets of overlapping peers, which would create duplicate information.
		 * Smart pointers are used, because a std::vector's memory addresses may change when more memory is required for an insert.
		 * Smart pointers ensure that the memory region which is pointed too will always be available.
		 */
		std::vector<PeerDataPtr> location_list;
	public:
		ObjectInfo();
		virtual ~ObjectInfo();

		/**
		 * Set the object name
		 *
		 * @param o_name A std::string value containing the object name
		 */
		void setObjectName(const std::string &o_name);

		/**
		 * Set the object name
		 *
		 * @param o_name A char* value containing the object name
		 */
		void setObjectName(char *o_name);

		std::string getObjectName();

		/**
		 * Add a TransportAddress to the list of object locations
		 *
		 * @param adr A TransportAddress containing the IP and port information
		 */
		void addPeerRef(PeerDataPtr peer_data_ptr);

		PeerDataPtr getPeerRef(const int &i);

		/**
		 * @returns a random address from the list of object locations
		 */
		PeerDataPtr getRandPeerRef();

		void setSize(const int &siz);
		int getSize();
};

#endif /* OBJECTINFO_H_ */
