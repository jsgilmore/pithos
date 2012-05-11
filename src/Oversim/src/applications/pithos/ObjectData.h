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

#ifndef OBJECTDATA_H_
#define OBJECTDATA_H_

#include <omnetpp.h>
#include <tr1/memory>

#include "OverlayKey.h"
#include "GameObject.h"

class ObjectData;

/**
 * ObjectDataPtr is a typedef for the shared_ptr smart pointer type, which using
 * reference counting to ensure that memory is never freed while there are still
 * pointers pointing to it. It also automatically frees memory if no pointers are
 * pointing to it anymore. Their use is required when creating pointers to dynamic
 * structures like vectors.
 */
typedef std::tr1::shared_ptr <ObjectData> ObjectDataPtr;

#define OBJECTDATA_SIZE 8+4+sizeof(OverlayKey)+sizeof(simtime_t)+4+4	//Object name ID (8B), size (4B), key, creation time (4B), ttl (4B), init group size (4B)

class ObjectData
{
private:
	/** Object name */
	std::string object_name;

	/** Object size in bytes */
	int size;

	OverlayKey key;

	simtime_t creationTime; /**< The time when the object was created */
	int init_group_size;	/**< The initial group size, when the object was inserted */
	int max_group_size;		/**< The maximum group size, while the object was alive */

	int ttl;				/**< The time-to-live of the object */

	friend std::ostream& operator<<(std::ostream& Stream, const ObjectData object_data);

public:

	static const ObjectData UNSPECIFIED_OBJECT;

	ObjectData(std::string name = "Unspecified" , int siz = 0, OverlayKey k = OverlayKey::ZERO, simtime_t time = SIMTIME_ZERO, int t = 0, int i_group_size = 0);
	ObjectData(const ObjectData& other);
	ObjectData(const GameObject& go, int i_group_size);
	virtual ~ObjectData();

	ObjectData& operator=(const ObjectData& other);

	friend bool operator==(const ObjectData& object1, const ObjectData& object2);
	friend bool operator!=(const ObjectData& object1, const ObjectData& object2);

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

	void setSize(const int &siz);

	int getSize();

	void setInitGroupSize(const int &siz);

	int getInitGroupSize();

	void setMaxGroupSize(const int &siz);

	int getMaxGroupSize();

	void setKey(const OverlayKey &k);

	OverlayKey getKey();

	void setCreationTime(simtime_t time);
	simtime_t getCreationTime();
	void setTTL(int t);
	int getTTL();

	bool isUnspecified();
};

#endif /* OBJECTDATA_H_ */
