//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef DHTSTORAGE_H_
#define DHTSTORAGE_H_

#include <omnetpp.h>

#include <SHA1.h>
#include <GlobalNodeList.h>
#include <GlobalStatistics.h>
#include <UnderlayConfigurator.h>
#include <TransportAddress.h>
#include <OverlayKey.h>
#include <InitStages.h>
#include <BinaryValue.h>
#include <BaseApp.h>

#include "BaseApp.h"
#include "Peer_logic.h"

#include "groupPkt_m.h"
#include "GameObject.h"

class GlobalDhtTestMap;

class DHTStorage : public BaseApp
{
	public:
		DHTStorage();
		virtual ~DHTStorage();
	private:
		/**
		 * A container used by the DHTTestApp to
		 * store context information for statistics
		 *
		 * @author Ingmar Baumgart
		 */
		class DHTStatsContext : public cPolymorphic
		{
			public:
				bool measurementPhase;
				simtime_t requestTime;
				OverlayKey key;
				BinaryValue value;

				DHTStatsContext(bool measurementPhase,
								simtime_t requestTime,
								const OverlayKey& key,
								const BinaryValue& value = BinaryValue::UNSPECIFIED_VALUE) :
					measurementPhase(measurementPhase), requestTime(requestTime),
					key(key), value(value) {};
		};

		UnderlayConfigurator* underlayConfigurator; /**< pointer to UnderlayConfigurator in this node */

		GlobalNodeList* globalNodeList; /**< pointer to GlobalNodeList in this node*/

		GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node*/
		GlobalDhtTestMap* globalDhtTestMap; /**< pointer to the GlobalDhtTestMap module */

		// parameters
		bool debugOutput; /**< debug output yes/no?*/
		int ttl; /**< ttl for stored DHT records */
		bool activeNetwInitPhase; //!< is app active in network init phase?

		// statistics
		int numSent; /**< number of sent packets*/
		int numGetSent; /**< number of get sent*/
		int numGetError; /**< number of false get responses*/
		int numGetSuccess; /**< number of false get responses*/
		int numPutSent; /**< number of put sent*/
		int numPutError; /**< number of error in put responses*/
		int numPutSuccess; /**< number of success in put responses*/

		bool nodeIsLeavingSoon; //!< true if the node is going to be killed shortly

		void handleTraceMessage(cMessage* msg);
		void handleRpcResponse(BaseResponseMessage* msg, const RpcState& state, simtime_t rtt);
		void handlePutResponse(DHTputCAPIResponse* msg, DHTStatsContext* context);
		void store(GameObject *go);
		void finishApp();
	protected:
		virtual void initializeApp(int stage);
		virtual void handleMessage(cMessage *msg);
};

Define_Module(DHTStorage);

#endif /* OVERLAYSTORAGE_H_ */
