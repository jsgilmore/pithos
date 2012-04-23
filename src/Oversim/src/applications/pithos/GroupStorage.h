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

#include "GroupLedger.h"
#include "PeerData.h"
#include "GameObject.h"
#include "PeerListPkt.h"
#include "PithosMessages_m.h"

class GlobalStatistics;
class GroupLedger;

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

		/**
		 * This class provides a means to store all information about pending requests sent to the group.
		 * A map of all pending requests are maintained and matched against responses received. If a response for
		 * a pending request is received, the original caller can be informed about the outcome of the request.
		 *
		 * @author John Gilmore
		 */
		class PendingRequestsEntry
		{
			public:
				PendingRequestsEntry()
				{
					numGetSent = 0;
					numPutSent = 0;
					numGroupPutFailed = 0;
					numGroupPutSucceeded = 0;
					numGroupGetFailed = 0;
					numGroupGetSucceeded = 0;
					responseType = UNSPECIFIED;
				};

				int numGetSent;
				int numPutSent;
				int numGroupPutFailed;
				int numGroupPutSucceeded;
				int numGroupGetFailed;
				int numGroupGetSucceeded;
				int responseType;

				//Smart pointers are not required here, since the pointers do not point to elements in dynamic containers
				std::vector<ResponseTimeoutEvent *> timeouts;
		};

		//friend std::ostream& operator<<(std::ostream& Stream, const PendingRequestsEntry& entry);

		typedef std::map<uint32_t, PendingRequestsEntry> PendingRequests;
		PendingRequests pendingRequests; /**< a map of all pending requests */

		char directory_ip[16]; /**< The IP address of the directory server (specified as an Omnet param value) */
		int directory_port; /**< The port of the directory server (specified as an Omnet param value) */

		/**< A map that stores all game objects on this group peer */
		typedef std::map<OverlayKey, GameObject> StorageMap;
		StorageMap storage_map;

		TransportAddress super_peer_address; /**< The TransPort address of the group super peer (this address is set, after the peer has joined a group) */
		TransportAddress this_address;		 /**< The TransPort address of the peer that houses the group storage module*/

		cMessage *event;	//This event is required

		double latitude; /**< The latitude of this peer (position in the virtual world) */

		double longitude; /**< The longitude of this peer (position in the virtual world) */

		PeerData lastPeerLeft;	/**< The peer data of the last peer that left the group */

		GroupLedger *group_ledger;

		GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node*/

		// statistics
		int numSent; /**< number of sent packets*/
		int numPutSent; /**< number of put sent*/
		int numGetSent; /**< number of put sent*/
		int numGetError; /**< number of error in put responses*/
		int numGetSuccess; /**< number of success in put responses*/
		int numPutError; /**< number of error in put responses*/
		int numPutSuccess; /**< number of success in put responses*/
		int numGetSecondGood;
		int	numGetSecondBad;

		//Get and Put error reasons
		int getErrMissingObjectSamePeer;
		int getErrMissingObjectOtherPeer;
		int getErrRequestOOG;
		int putErrStoreOOG;

		//Request settings
		simtime_t requestTimeout;	/**< The amount of time to wait for a response to a request, before a node is removed from the group*/
		int numGetRequests;

		bool gracefulMigration;

		//Whether or not to repair, as well as the repair type
		bool objectRepair;
		bool periodicRepair;

		/**
		 * The function creates a write packet and fills it with address information, payload type and byte length.
		 *
		 * @param write A pointer to the write packet to be created and filled
		 * @param rpcid The RPC ID of the original request from the higher layer
		 */
		void createWritePkt(ValuePkt **write, unsigned int rpcid);

		//void addObject(cMessage *msg);

		/**
		 * Send a message to the super peer informing it about a new group object and a list of peers that store it.
		 *
		 * @param go The GameObject stored, of which the size and name are used.
		 * @param send_list A history of all peers selected to store the new object.
		 */
		void updatePeerObjects(const GameObject& go);

		/**
		 * Select a random TransportAddress within the group that has not be chosen for a replica during the current selection process.
		 *
		 * @param dest_adr The selected destination address to which an object should be written.
		 * @param send_list A history of peers selected to store the particular GameObject.
		 */
		PeerData selectDestination(std::vector<TransportAddress> send_list);

		void handleResponse(PendingRequests::iterator it, ResponsePkt *response);
		/**
		 * Cancel the pending request timeout
		 * @returns the peer data related to the request
		 */
		PeerData cancelRequestTimer(PendingRequests::iterator it, TransportAddress source_address);

		void respond_toUpper(cMessage *msg);

		void createResponseMsg(ResponsePkt **response, int responseType, unsigned int rpcid, bool isSuccess, const GameObject& object);

		void sendUDPResponse(TransportAddress src_adr, TransportAddress dest_adr, int responseType, unsigned int rpcid, bool isSuccess, const GameObject& object = GameObject::UNSPECIFIED_OBJECT);

		void sendUpperResponse(int responseType, unsigned int rpcid, bool isSuccess, const GameObject& object = GameObject::UNSPECIFIED_OBJECT);

		/**
		 * Send a join request to the directory server or a super peer
		 *
		 * @param the destination address of the directory server of super peer
		 */
		void joinRequest(const TransportAddress &dest_adr);

		void replicateLocalObjects();

		void leaveGroup();

		/**
		 * Handle a message received from the group over UDP
		 *
		 * @param msg the message received
		 */
		void addAndJoinSuperPeer(Packet *packet);

		void store(Packet *pkt);

		void handleLeftPeer(PeerDataPkt *peer_data_pkt);
		/**
		 * Function to store files in the group. This function replicates game objects and sends them to different group nodes.
		 *
		 * @param store_req The received storage request message
		 */
		void send_forstore(ValuePkt *store_req);

		void forwardRequest(OverlayKeyPkt *retrieve_req);
		bool handleMissingObject(OverlayKeyPkt *retrieve_req);
		bool retrieveLocally(OverlayKeyPkt *retrieve_req);
		void requestRetrieve(OverlayKeyPkt *retrieve_req);

		void replicate(ObjectData object_data, int repplica_diff);

		/**
		 * Function is called when the peer is informed by the group super peer that new peers have joined the group.
		 *
		 * @param msg A message containing the information of the new peers and what objects they host.
		 */
		void addToGroup(cMessage *msg);

		void removePeer(PeerDataPtr peerDataPtr);

		void peerLeftInform(PeerData peerData, int sp_way_left);

		/**
		 * @returns the number of required replicas or the number of group peers, if this number is less than the required replicas (this case is also logged).
		 */
		int getReplicaNr(unsigned int rpcid);

		void removePeer(Packet *packet);

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
		void handleTimeout(ResponseTimeoutEvent *timeout);
		void handlePacket(Packet *packet);
		virtual void handleMessage(cMessage *msg);
};

#endif /* OVERLAYSTORAGE_H_ */
