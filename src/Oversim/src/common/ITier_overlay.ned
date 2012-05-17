
package oversim.common;

//
// Interface for tier compound modules with an attached overlay
//
// @author Ingmar Baumgart, Bernhard Heep, Stephan Krause
//
moduleinterface ITier_overlay
{
    parameters:
        @display("i=block/segm");
        
    gates:
        input udpIn;    // gate from the UDP layer
        input from_lowerTier;    // gate from the lower tier
        input from_upperTier;    // gate from the upper tier
        input tcpIn;    // gate from the TCP layer
        output tcpOut;    // gate to the TCP layer
        output udpOut;    // gate to the UDP layer
        output to_lowerTier;    // gate to the lower tier
        output to_upperTier;    // gate to the upper tier
        input overlayIn;    // gate from the TCP layer
        output overlayOut;    // gate to the TCP layer
}