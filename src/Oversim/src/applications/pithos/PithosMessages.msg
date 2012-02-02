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

cplusplus {{
#include <TransportAddress.h>
#include "PeerData.h"
#include "ObjectData.h"
#include "OverlayKey.h"
}}

// import the IP Address class
class noncobject TransportAddress;
class noncobject OverlayKey;
class noncobject PeerData;
class noncobject ObjectData;
class noncobject PeerDataPtr;

enum PacketTypes
{
    INFORM = 1;
    WRITE = 2;				//A UDP packet sent over the group network, containing a game object to be stored.
    OVERLAY_WRITE_REQ = 3;
    SUPER_PEER_ADD = 4;
    JOIN_REQ = 5;
    JOIN_ACCEPT = 6;
    STORE_REQ = 7;			//A packet sent from peer logic to the lower layers, requesting an object be stored
    RETRIEVE_REQ = 8;		//A packet sent from peer logic to the lower layers, requesting an object be retrieved
    OBJECT_ADD = 9;
    SP_OBJECT_ADD = 10;
    RESPONSE = 11;
    PEER_LEFT = 12;
    SP_PEER_LEFT = 13;
};

enum OverlayTypes 
{
    OVERLAY_WRITE = 1;           // Overlay write
    OVERLAY_READ = 2;           // Overlay read
}

enum ResponseTypes 
{
    UNSPECIFIED = 0;
    OVERLAY_PUT = 1;
    OVERLAY_GET = 2;
    OVERLAY_MOD = 3;
    GROUP_PUT = 4;
    GROUP_GET = 5;
    GROUP_MOD = 6;
    
}

// now we declare the message
packet overlayPkt
{
    int type enum(OverlayTypes);      	// message type
}

packet AddressPkt
{
    TransportAddress address;
}

packet Packet
{
    TransportAddress sourceAddress;
    TransportAddress destinationAddress;
    
    int payloadType enum(PacketTypes);
}

packet ValuePkt extends Packet
{
    unsigned int value;
}

packet OverlayKeyPkt extends Packet
{
    unsigned int value;
    OverlayKey key;
}

message ResponsePkt extends Packet
{
    unsigned int rpcid;
    bool isSuccess;
    TransportAddress groupAddress;
    int responseType enum(ResponseTypes);
}

packet bootstrapPkt extends Packet
{
    TransportAddress superPeerAdr;
    double latitude;
    double longitude;
}

packet PeerListPkt extends Packet
{
    @customize(true);
    
    //This string is actually modelled as a long long int (64 bits)
    //for the purposes of packet transmission, but a string is used
    //to make the stored object easily readable
    ObjectData objectData;
    
    abstract PeerData peer_list[];
}

packet PeerDataPkt extends Packet
{
    PeerData peerData;
}

message ResponseTimeoutEvent
{
    unsigned int rpcid;
    PeerDataPtr peerDataPtr;
}