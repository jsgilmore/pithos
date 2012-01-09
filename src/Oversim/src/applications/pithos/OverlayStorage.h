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

#ifndef OVERLAYSTORAGE_H_
#define OVERLAYSTORAGE_H_

#include <omnetpp.h>

#include "BaseApp.h"
#include "GroupStorage.h"

#include "PithosMessages_m.h"
#include "GameObject.h"

/**
 * The OverlayStorage module is responsible for handling all aspects of overlay
 * storage on the sending side. When another module wants to store an object
 * in overlay storage, that module sends the object to this module. The reason
 * for this indirection is that it allows for a modular design of overlay storage
 * and the further implementation of other types of overlay storage.
 *
 * @author John Gilmore
 */
class OverlayStorage : public cSimpleModule
{
	public:
		OverlayStorage();
		virtual ~OverlayStorage();
	public:

		/**
		 * Push a GameObject to the overlay for storage
		 *
		 * @param go the GameObject to be pushed to the overlay
		 */
		void store(GameObject *go);

	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
};

#endif /* OVERLAYSTORAGE_H_ */
