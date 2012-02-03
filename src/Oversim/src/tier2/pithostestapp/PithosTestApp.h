//
// Copyright (C) 2007 Institut fuer Telematik, Universitaet Karlsruhe (TH)
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

/**
 * @file PithosTestApp.h
 * @author John Gilmore Ingmar Baumgart
 */

#ifndef __PITHOSTESTAPP_H_
#define __PITHOSTESTAPP_H_

#include <omnetpp.h>

#include <GlobalNodeList.h>
#include <GlobalStatistics.h>
#include <UnderlayConfigurator.h>
#include <TransportAddress.h>
#include <OverlayKey.h>
#include <InitStages.h>
#include <BinaryValue.h>
#include <BaseApp.h>
#include <set>
#include <sstream>

#include "PithosTestMessages_m.h"
#include "PithosMessages_m.h"
#include "GlobalPithosTestMap.h"
#include "GameObject.h"

class GlobalPithosTestMap;

/**
 * A simple test application for the DHT layer
 *
 * A simple test application that does random put and get calls
 * on the DHT layer
 *
 * @author John Gilmore
 * @author Ingmar Baumgart
 */
class PithosTestApp : public BaseApp
{
private:
    /**
     * A container used by the PithosTestApp to
     * store context information for statistics
     *
     * @author Ingmar Baumgart
     */
    class PithosStatsContext : public cPolymorphic
    {
		public:
			bool measurementPhase;
			simtime_t requestTime;
			OverlayKey key;
			GameObject go;

			PithosStatsContext(bool measurementPhase, simtime_t requestTime, const GameObject& go = GameObject::UNSPECIFIED_OBJECT) :
				measurementPhase(measurementPhase), requestTime(requestTime), key(OverlayKey::UNSPECIFIED_KEY), go(go) {};
			PithosStatsContext(bool measurementPhase, simtime_t requestTime, const OverlayKey& key) :
							measurementPhase(measurementPhase), requestTime(requestTime), key(key), go(GameObject::UNSPECIFIED_OBJECT) {};
    };

    void initializeApp(int stage);

    /**
     * Get a random key of the hashmap
     */
    OverlayKey getRandomKey();

    OverlayKey getKey();

    void finishApp();

    void handleLowerMessage (cMessage *msg);

    void sendPutRequest();

    /**
     * processes get responses
     *
     * method to handle get responses
     * should be overwritten in derived application if needed
     * @param msg get response message
     * @param context context object used for collecting statistics
     */
    void handleGetResponse(RootObjectGetCAPIResponse* msg, PithosStatsContext* context);

    /**
     * processes put responses
     *
     * method to handle put responses
     * should be overwritten in derived application if needed
     * @param msg put response message
     * @param context context object used for collecting statistics
     */
    void handlePutResponse(RootObjectPutCAPIResponse* msg, PithosStatsContext* context);

    /**
     * processes self-messages
     *
     * method to handle self-messages
     * should be overwritten in derived application if needed
     * @param msg self-message
     */
    virtual void handleTimerEvent(cMessage* msg);

    virtual void handleNodeLeaveNotification();

    // see RpcListener.h
    void handleRpcResponse(BaseResponseMessage* msg, const RpcState& state, simtime_t rtt);

    UnderlayConfigurator* underlayConfigurator; /**< pointer to UnderlayConfigurator in this node */

    GlobalNodeList* globalNodeList; /**< pointer to GlobalNodeList in this node*/

    GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node*/
    GlobalPithosTestMap* globalPithosTestMap; /**< pointer to the GlobalPithosTestMap module */

    // parameters
    bool debugOutput; /**< debug output yes/no?*/
    double mean; //!< mean time interval between sending test messages
    double deviation; //!< deviation of time interval
    int ttl; /**< ttl for stored Pithos records */
    bool activeNetwInitPhase; //!< is app active in network init phase?
    int groupProbability;

    simtime_t writeTime_av;
	simtime_t wait_time;
	simtime_t join_time;
	simtime_t generationTime;
	double objectSize_av;

    // statistics
    int numSent; /**< number of sent packets*/
    int numGetSent; /**< number of get sent*/
    int numGetError; /**< number of false get responses*/
    int numGetSuccess; /**< number of false get responses*/
    int numPutSent; /**< number of put sent*/
    int numPutError; /**< number of error in put responses*/
    int numPutSuccess; /**< number of success in put responses*/

    int numGroupGet; /**< number of group gets*/
    int numOverlayGet; /**< number of overlay gets*/

    cMessage *pithostestput_timer, *pithostestget_timer, *pithostestmod_timer;
    bool nodeIsLeavingSoon; //!< true if the node is going to be killed shortly

    static const int DHTTESTAPP_VALUE_LEN = 20;

    TransportAddress super_peer_address;

public:
    PithosTestApp();

    /**
     * virtual destructor
     */
    virtual ~PithosTestApp();

};

#endif
