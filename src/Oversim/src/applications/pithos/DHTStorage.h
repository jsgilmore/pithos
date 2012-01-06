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

#include <GlobalNodeList.h>
#include <GlobalStatistics.h>
#include <UnderlayConfigurator.h>
#include <TransportAddress.h>
#include <OverlayKey.h>
#include <InitStages.h>
#include <BinaryValue.h>
#include <BaseApp.h>

#include "BaseApp.h"
#include "Communicator.h"
#include "Peer_logic.h"

#include "PithosMessages_m.h"
#include "GameObject.h"

class Peer_logic;
class Communicator;
class GlobalStatistics;
class GlobalDhtTestMap;

class DHTStorage : public cSimpleModule
{
	public:
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
				unsigned int parent_rpcid;
				GameObject object;

				DHTStatsContext(bool measurementPhase, simtime_t requestTime, const OverlayKey& key, unsigned int parent_rpcid=0, const GameObject& object = GameObject::UNSPECIFIED_OBJECT) :
					measurementPhase(measurementPhase), requestTime(requestTime), key(key), parent_rpcid(parent_rpcid), object(object) {};
		};

		DHTStorage();
		virtual ~DHTStorage();

		void handleRpcResponse(BaseResponseMessage* msg, const RpcState& state, simtime_t rtt);
		void handleTraceMessage(cMessage* msg);
	private:

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
		int numPutSent; /**< number of put sent*/
		int numGetSent; /**< number of put sent*/
		int numGetError; /**< number of error in put responses*/
		int numGetSuccess; /**< number of success in put responses*/
		int numPutError; /**< number of error in put responses*/
		int numPutSuccess; /**< number of success in put responses*/

		bool nodeIsLeavingSoon; //!< true if the node is going to be killed shortly

		void request_retrieve(Packet *pkt);

		void send_forstore(Packet *pkt);

		void sendResponse(int responseType, unsigned int rpcid, bool isSuccess, const BinaryValue& value = BinaryValue::UNSPECIFIED_VALUE);

	protected:
		void finish();
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);

		void handleGetResponse(DHTgetCAPIResponse* msg, DHTStatsContext* context);
		void handlePutResponse(DHTputCAPIResponse* msg, DHTStatsContext* context);
};

#endif /* OVERLAYSTORAGE_H_ */
