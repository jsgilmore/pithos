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

#ifndef GROUPSTORAGE_H_
#define GROUPSTORAGE_H_

#include <omnetpp.h>
#include <GlobalStatistics.h>


#include "BaseApp.h"
#include "Peer_logic.h"
#include "Communicator.h"

#include "GameObject.h"
#include "PeerListPkt.h"
#include "PithosMessages_m.h"

class GlobalStatistics;

/**
 * The GroupStorage module is responsible for handling all aspects of group
 * storage on the sending side. When another module wants to store an object
 * in group storage, that module sends the object to this module. The reason
 * for this indirection is that it allows for a modular design of group storage
 * and the further implementation of other types of group storage.
 *
 * @author John Gilmore
 */
class GroupStorage : public cSimpleModule
{
	public:
	GroupStorage();
		virtual ~GroupStorage();
		int getStorageBytes();
		int getStorageFiles();
	private:

		cQueue storage; /**< The queue holding all stored GameObjects */

		std::vector<PeerData> group_peers; /**< The list of peers in the group and which objects they store. */

		simsignal_t groupSizeSignal; /**< The signal recording the statistic of the group size. */

		simsignal_t groupSendFailSignal; /**< The signal recording the statistic of how many times a group storage failed. */

		simsignal_t joinTimeSignal; /**< The signal recording the statistic of the group size. */

		GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node*/

		// statistics
		//int numSent; /**< number of sent packets*/
		//int numPutSent; /**< number of put sent*/
		//int numPutError; /**< number of error in put responses*/
		//int numPutSuccess; /**< number of success in put responses*/

		/**
		 * The function creates a write packet and fills it with address information, payload type and byte length.
		 *
		 * @param write A pointer to the write packet to be created and filled
		 * @param rpcid The RPC ID of the original request from the higher layer
		 */
		void createWritePkt(ValuePkt **write, unsigned int rpcid);

		/**
		 * Send a message to the super peer informing it about a new group object and a list of peers that store it.
		 *
		 * @param objectName Name of the newly stored object.
		 * @param objectSize Size of the newly stored object in bytes.
		 * @param send_list A history of all peers selected to store the new object.
		 */
		void updateSuperPeerObjects(const char *objectName, unsigned long objectSize, std::vector<TransportAddress> send_list);

		/**
		 * Select a random TransportAddress within the group that has not be chosen for a replica during the current selection process.
		 *
		 * @param dest_adr The selected destination address to which an object should be written.
		 * @param send_list A history of peers selected to store the particular GameObject.
		 */
		void selectDestination(TransportAddress *dest_adr, std::vector<TransportAddress> send_list);

		void respond_toUpper(cMessage *msg);

		void sendUDPResponse(cMessage *msg);

		void store(cMessage *msg);

		/**
		 * Function to store files in the group. This function replicates game objects and sends them to different group nodes.
		 *
		 * @param go The GameObject to be stored
		 * @param rpcid The RPC ID of the original request from the higher layer
		 */
		void send_forstore(GameObject *go, unsigned int rpcid);

		/**
		 * Function is called when the peer is informed by the group super peer that new peers have joined the group.
		 *
		 * @param msg A message containing the information of the new peers and what objects they host.
		 */
		void addPeers(cMessage *msg);

		/**
		 * @returns the number of required replicas or the number of group peers, if this number is less than the required replicas (this case is also logged).
		 */
		int getReplicaNr(unsigned int rpcid);
	protected:
		simsignal_t qlenSignal; /**< Signal for recording the number of objects stored */

		simsignal_t qsizeSignal; /**< Signal for recording the size of objects stored in bytes */

		simsignal_t overlayObjectsSignal; /**< Signal for recording the number of overlay objects stored */

		simsignal_t rootObjectsSignal; /**< Signal for recording the number of root objects stored */

		simsignal_t replicaObjectsSignal; /**< Signal for recording the number of group replica objects stored */

		simsignal_t storeTimeSignal; /**< Signal for recording the time that was required to store each object */

		simsignal_t rootStoreTimeSignal; /**< Signal for recording the time that was required to store a root object */

		simsignal_t replicaStoreTimeSignal; /**< Signal for recording the time that was required to store a group replica object */

		simsignal_t overlayStoreTimeSignal; /**< Signal for recording the time that was required to store an overlay object */

		void finishApp();
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
};

#endif /* OVERLAYSTORAGE_H_ */
