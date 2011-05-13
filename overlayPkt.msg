// first, we declare an enum to tell the message types apart:
enum OverlayTypes 
{
    OVERLAY_WRITE = 1;           // Overlay write
    OVERLAY_READ = 2;           // Overlay read
}

// now we declare the message
packet OverlayPkt
{
    int type enum(OverlayTypes);      	// message type
}