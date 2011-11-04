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

#ifndef GAME_H_
#define GAME_H_

#include <omnetpp.h>
#include <BaseApp.h>

#include "groupPkt_m.h"

/**
 * Class that is used by the Omnet module of the same name, the purpose of
 * which is to generate data storage, retrieval and update requests to
 * stimulate the lower storage layer.
 *
 * @author John Gilmore
 */
class Game : public BaseApp
{
	private:
		simtime_t writeTime_av;
		simtime_t wait_time;
		simtime_t join_time;
		simtime_t generationTime;
		double objectSize_av;
		cMessage *event;
	public:
		Game();
		virtual ~Game();
	protected:

        /** Called when the module is being created */
		void initializeApp(int stage);

        /** Called when the module is about to be destroyed */
		void finishApp();

        /**
         * Called when a timer message is received to produce another request
         *
         * @param msg The timer message
         */
		void handleTimerEvent(cMessage* msg);

		/**
		 * Handles a message from the storage layer, informing the game
		 * module that the P2P connection has been established and can
		 * start generating requests.
		 *
		 * @param msg The message which just acts as a signal for the game module to start requests generation.
		 */
		void handleLowerMessage (cMessage *msg);

		/**
		 * Called when a message is received from the overlay. This should not happen.
		 *
		 * @param key The overlay routing key
		 * @param msg The message received from the overlay.
		 *
		 */
		void deliver(OverlayKey& key, cMessage* msg);

		/**
		 * Send a store request to the lower storage layer. Currently the game module only sends store requests.
		 */
		void sendRequest();
};

Define_Module(Game);

#endif /* GAME_H_ */
