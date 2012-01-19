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

#include "ObjectInfo.h"
#include "PeerData.h"
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

		bool hasSuperPeer();
		TransportAddress getSuperPeerAddress();

	private:

		cMessage *event; /**< An event used to trigger a join request */

		char directory_ip[16]; /**< The IP address of the directory server (specified as a Omnet param value) */

		int directory_port; /**< The port of the directory server (specified as a Omnet param value) */

		cQueue storage; /**< The queue holding all stored GameObjects */

		/**< A map that stores all game objects on this group peer */
		typedef std::map<OverlayKey, GameObject> StorageMap;
		StorageMap storage_map;

		TransportAddress super_peer_address; /**< The TransPort address of the group super peer (this address is set, after the peer has joined a group) */

		double latitude; /**< The latitude of this peer (position in the virtual world) */

		double longitude; /**< The longitude of this peer (position in the virtual world) */

		std::vector<PeerDataPtr> group_peers; /**< A vector that records all peers that belong to this super peer's group */

		/**< A map that records all objects information stored in this super peer's group */
		typedef std::map<OverlayKey, ObjectInfo> ObjectInfoMap;
		ObjectInfoMap object_map;

		GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node*/

		// statistics
		int numSent; /**< number of sent packets*/
		int numPutSent; /**< number of put sent*/
		int numGetSent; /**< number of put sent*/
		int numGetError; /**< number of error in put responses*/
		int numGetSuccess; /**< number of success in put responses*/
		int numPutError; /**< number of error in put responses*/
		int numPutSuccess; /**< number of success in put responses*/

		/**
		 * The function creates a write packet and fills it with address information, payload type and byte length.
		 *
		 * @param write A pointer to the write packet to be created and filled
		 * @param rpcid The RPC ID of the original request from the higher layer
		 */
		void createWritePkt(ValuePkt **write, unsigned int rpcid);

		void addObject(cMessage *msg);

		/**
		 * Send a message to the super peer informing it about a new group object and a list of peers that store it.
		 *
		 * @param go The GameObject stored, of which the size and name are used.
		 * @param send_list A history of all peers selected to store the new object.
		 */
		void updatePeerObjects(GameObject go);

		/**
		 * Select a random TransportAddress within the group that has not be chosen for a replica during the current selection process.
		 *
		 * @param dest_adr The selected destination address to which an object should be written.
		 * @param send_list A history of peers selected to store the particular GameObject.
		 */
		void selectDestination(TransportAddress *dest_adr, std::vector<TransportAddress> send_list);

		void respond_toUpper(cMessage *msg);

		void createResponseMsg(ResponsePkt **response, int responseType, unsigned int rpcid, bool isSuccess, GameObject object);

		void sendUDPResponse(TransportAddress src_adr, TransportAddress dest_adr, int responseType, unsigned int rpcid, bool isSuccess, GameObject object = GameObject::UNSPECIFIED_OBJECT);

		void sendUpperResponse(int responseType, unsigned int rpcid, bool isSuccess, GameObject object = GameObject::UNSPECIFIED_OBJECT);

		/**
		 * Send a join request to the directory server or a super peer
		 *
		 * @param the destination address of the directory server of super peer
		 */
		void joinRequest(const TransportAddress &dest_adr);

		/**
		 * Handle a message received from the group over UDP
		 *
		 * @param msg the message received
		 */
		void addAndJoinSuperPeer(Packet *packet);

		void store(cMessage *msg);

		/**
		 * Function to store files in the group. This function replicates game objects and sends them to different group nodes.
		 *
		 * @param go The GameObject to be stored
		 * @param rpcid The RPC ID of the original request from the higher layer
		 */
		void send_forstore(GameObject *go, unsigned int rpcid);

		void requestRetrieve(OverlayKeyPkt *retrieve_req);

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

		simsignal_t objectsSignal; /**< Signal for recording the number of root objects stored */

		simsignal_t storeTimeSignal; /**< Signal for recording the time that was required to store each object */

		simsignal_t groupSizeSignal; /**< The signal recording the statistic of the group size. */

		simsignal_t groupSendFailSignal; /**< The signal recording the statistic of how many times a group storage failed. */

		simsignal_t joinTimeSignal; /**< The signal recording the statistic of the group size. */

		void finish();
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
};

#endif /* OVERLAYSTORAGE_H_ */
