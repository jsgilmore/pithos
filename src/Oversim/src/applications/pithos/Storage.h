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
#ifndef STORAGE_H_
#define STORAGE_H_

#include <omnetpp.h>

#include "GameObject.h"

/**
 * Any data that is stored in Pitos is stored in the storage class.
 * The class contains a queue to which game objects may be added and
 * collects statistics on the objects stored.
 *
 * @author John Gilmore
 */
class Storage : public cSimpleModule
{
	public:
	Storage();
		virtual ~Storage();
		int getStorageBytes();
		int getStorageFiles();
	private:

		cQueue storage; /**< The queue holding all stored GameObjects */
	protected:

		simsignal_t qlenSignal; /**< Signal for recording the number of objects stored */

		simsignal_t qsizeSignal; /**< Signal for recording the size of objects stored in bytes */

		simsignal_t objectsSignal; /**< Signal for recording the number of root objects stored */

		simsignal_t storeTimeSignal; /**< Signal for recording the time that was required to store each object */


		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
};

#endif /* PEER_H_ */
