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

#include "BinaryValue.h"

enum ObjectTypes {
    ROOT = 1,
    REPLICA = 2,
    OVERLAY = 3
};

/**
 * The game object class stores the information of a game object.
 * These are the atomic objects that are stored in Pithos.
 *
 * @author John Gilmore
 */
class GameObject : public cOwnedObject
{
	private:

		/**
		 * The name of the object
		 */
		char objectName[41];

		/**
		 * The size in bytes of the object
		 */
		int64_t size;

		/**
		 * The type of object (root,replica,overlay)
		 */
		int type;

		/**
		 * The time when the object was created
		 */
		simtime_t creationTime;

	public:
		GameObject(const char *name="", int o_type=ROOT, int64_t o_size=0, simtime_t o_creationTime=0);
		GameObject(const GameObject& other);
		GameObject(const BinaryValue& binval);
		virtual ~GameObject();
		GameObject& operator=(const GameObject& other);
		virtual std::string info();

		/** @returns Duplicate of the game object */
		virtual GameObject *dup() const;

		/** @returns the size of the object in bytes */
		int64_t getSize();

		/** @param o_size The size of the object in bytes */
		void setSize(const int64_t &o_size);
		int getType();
		void setType(const int &o_type);
		void setObjectName(const char *o_Name);
		char *getObjectName();
		void setCreationTime(const simtime_t &time);
		simtime_t getCreationTime();

		BinaryValue getBinaryValue();
};

#endif /* GO_H_ */
