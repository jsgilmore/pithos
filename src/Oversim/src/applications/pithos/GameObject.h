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

#ifndef GO_H_
#define GO_H_

#include <omnetpp.h>
#include <SHA1.h>
#include <TransportAddress.h>

#include "OverlayKey.h"
#include "BinaryValue.h"

/**
 * The game object class stores the information of a game object.
 * These are the atomic objects that are stored in Pithos.
 *
 * @author John Gilmore
 */
class GameObject : public cOwnedObject
{
	private:

		//char objectName[41];
		std::string objectName; /**< The name of the game object, which is different from the name of the object itself, which is usually "GameObject" */

		int64_t size; 			/**< The size in bytes of the object */

		simtime_t creationTime; /**< The time when the object was created */
		int ttl;				/**< The time-to-live of the object */

		TransportAddress group_address;

		int value;	//This variable represents the data contained in the game object

		friend std::ostream& operator<<(std::ostream& Stream, const GameObject entry);

	public:
		static const GameObject UNSPECIFIED_OBJECT;

		//If no ttl is given and the object is stored, it will not be able to exist in storage
		//creationTime is initialised to an empty simtime_t object. Values such as zero do not work, since the scale exponent for simtime_t might not have been defined.
		GameObject(const std::string objectName = "GameObject", int64_t o_size=0, simtime_t o_creationTime=SIMTIME_ZERO, int o_ttl=0);
		GameObject(const GameObject& other);
		GameObject(const BinaryValue& binval);
		virtual ~GameObject();
		GameObject& operator=(const GameObject& other);
		GameObject& operator=(const BinaryValue& binval);
		friend bool operator==(const GameObject& object1, const GameObject& object2);
		friend bool operator!=(const GameObject& object1, const GameObject& object2);

		//This is used when a GameObject is the key in a map
		friend bool operator<(const GameObject& object1, const GameObject& object2);
		virtual std::string info();
		virtual std::string info() const;

		//The name hash is used to store and retrieve objects in the system.
		OverlayKey getNameHash();
		OverlayKey getNameHash() const;

		//The content hash is used to compare the contents of different object, removing the need for the complete object to be transferred.
		OverlayKey getContentHash();
		OverlayKey getContentHash() const;

		/** @returns Duplicate of the game object */
		virtual GameObject *dup() const;

		/** @returns the size of the object in bytes */
		int64_t getSize();
		int64_t getSize() const;

		/** @param o_size The size of the object in bytes */
		void setSize(const int64_t &o_size);

		int getTTL() const;
		int getTTL();
		void setTTL(const int &o_ttl);

		int getValue();
		int getValue() const;
		void setValue(const int &val);

		void setObjectName(const std::string& o_Name);

		std::string getObjectName();
		std::string getObjectName() const;

		void setCreationTime(const simtime_t &time);

		simtime_t getCreationTime();
		simtime_t getCreationTime() const;

		BinaryValue getBinaryValue();
		BinaryValue getBinaryValue() const;

		TransportAddress getGroupAddress();
		TransportAddress getGroupAddress() const;
		void setGroupAddress(const TransportAddress &gr_adr);

		bool isUnspecified();
};

#endif /* GO_H_ */
