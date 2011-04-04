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

package oversim.applications.Pithos;

import oversim.common.BaseApp;
import oversim.common.ITier;

simple Game
{
    parameters:
        @class(Game);
        @display("i=block/source");
        double avWriteTime @unit(s) = default(0.001s);	//0.1 ms average
        double avObjectSize = default(1024);	//1 KB average
    gates:
        output write;
}

simple Storage
{
    parameters:
        @class(Storage);
        @display("i=abstract/cache");
        @signal[qlen](type="int");
        @signal[qsize](type="int");
        @signal[queueingTime](type="simtime_t");
        @statistic[qlen](title="queue length";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[qsize](title="queue size";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";unit=s;interpolationmode=none);
        
        @signal[OverlayObject](type="int");
        @signal[RootObject](type="int");
        @signal[ReplicaObject](type="int");
        @statistic[OverlayObject](title="Overlay Objects";record=sum);
        @statistic[RootObject](title="Root Objects";record=sum);
        @statistic[ReplicaObject](title="Replica Objects";record=sum);
    gates:
        output read;
        input write;
}

simple Peer_logic
{
    parameters:
        @class(Peer_logic);
        @display("i=block/cogwheel");
        @signal[busy](type="bool");
        @statistic[busy](title="server busy state";record=vector,timeavg;interpolationmode=sample-hold);
        int network_size = 21;
        int replicas_g = 3;
        int replicas_sp = 2;
    gates:
        output out[];
        input in[];
        output write;
        input read;
        input request;
        inout sp_gate @loose;
}

simple Super_peer_logic extends BaseApp
{
    parameters:
        @class(Super_peer_logic);
        @display("i=block/cogwheel");
        int network_size = 21;
        @signal[OverlayWrite](type="int");
        @statistic[OverlayWrite](title="Overlay writes";record=sum);
        int largestKey;
	gates:
	    inout overlay_gate[] @loose;
	    inout peer_gate;
}

module Peer
{
    parameters:
        @display("i=device/pc");
    gates:
        input in[];
        output out[];
    submodules:
        peer_logic: Peer_logic;
        storage: Storage;
        game: Game;
    connections:
        game.write --> peer_logic.request;
        peer_logic.write --> storage.write;
        storage.read --> peer_logic.read;
        for i=0..sizeof(out)-1 {
            peer_logic.out++ --> out[i];
            in[i] --> peer_logic.in++;
        }
}

module Super_peer like ITier
{
    parameters:
        @display("i=device/pc_l");
    gates:
        input in[];				 // gate to the local P2P group
        output out[];			 // gate from the local P2P group
        
        input udpIn;             // gate from the UDP layer
        output udpOut;           // gate to the UDP layer
        input from_lowerTier;    // gate from the lower tier
        input from_upperTier;    // gate from the upper tier
        output to_lowerTier;     // gate to the lower tier
        output to_upperTier;     // gate to the upper tier
        input trace_in;          // gate for trace file commands
        input tcpIn;             // gate from the TCP layer
        output tcpOut;           // gate to the TCP layer
    submodules:
        peer_logic: Peer_logic;
        super_peer_logic: Super_peer_logic;
        storage: Storage;
        game: Game;
    connections allowunconnected:
        //Internal connection to the peer_logic, storage and game modules
        peer_logic.sp_gate <--> super_peer_logic.peer_gate;
        game.write --> peer_logic.request;
        peer_logic.write --> storage.write;
        storage.read --> peer_logic.read;
        
        //Connections to the external local group peers
        for i=0..sizeof(out)-1 {
            peer_logic.out++ --> out[i];
            in[i] --> peer_logic.in++;
        }

		//Connections to the Oversim simulation modules
        from_lowerTier --> super_peer_logic.from_lowerTier;
        to_lowerTier <-- super_peer_logic.to_lowerTier;
        udpIn --> super_peer_logic.udpIn;
        udpOut <-- super_peer_logic.udpOut;
        tcpIn --> super_peer_logic.tcpIn;
        tcpOut <-- super_peer_logic.tcpOut;
        trace_in --> super_peer_logic.trace_in;
}

module Local_group like ITier
{
    parameters:
        @display("i=block/network");
    gates:
        input udpIn;             // gate from the UDP layer
        output udpOut;           // gate to the UDP layer
        input from_lowerTier;    // gate from the lower tier
        input from_upperTier;    // gate from the upper tier
        output to_lowerTier;     // gate to the lower tier
        output to_upperTier;     // gate to the upper tier
        input trace_in;          // gate for trace file commands
        input tcpIn;             // gate from the TCP layer
        output tcpOut;           // gate to the TCP layer
    submodules:
        peer[20]: Peer;
        super_peer: Super_peer;
    connections allowunconnected:
        //Connections to the Oversim simulation modules
        from_lowerTier --> super_peer.from_lowerTier;
        to_lowerTier <-- super_peer.to_lowerTier;
        udpIn --> super_peer.udpIn;
        udpOut <-- super_peer.udpOut;
        tcpIn --> super_peer.tcpIn;
        tcpOut <-- super_peer.tcpOut;
        trace_in --> super_peer.trace_in;
        
        for j=0..18, for i=j..18
        {
            peer[j].out++ --> {  datarate = 384Kbps; } --> peer[i+1].in++;
            peer[i+1].out++ --> {  datarate = 384Kbps; } --> peer[j].in++;
        }
        
        for i=0..19 {
            super_peer.out++ --> {  datarate = 384Kbps; } --> peer[i].in++;
            peer[i].out++ --> {  datarate = 384Kbps; } --> super_peer.in++;
        }
}
