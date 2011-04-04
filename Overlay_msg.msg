cplusplus {{
#include <TransportAddress.h>
}}


// import the IP Address class
class noncobject TransportAddress;

// first, we declare an enum to tell the message types apart:
enum PacketTypes 
{
    OVERLAY_WRITE = 1;           // Overlay write
    OVERLAY_READ = 2;           // Overlay read
}

// now we declare the message
packet OverlayMsg 
{
    int type enum(PacketTypes);      	// message type
    TransportAddress senderAddress;  	// address of the node that sent the message
    TransportAddress superPeerAddress;  // address of the super peer that relayed the request
}