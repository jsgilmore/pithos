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

package oversim.applications.pithos;

import oversim.common.BaseApp;
import oversim.common.ITier;

simple Game extends BaseApp
{
    parameters:
        @class(Game);
        @display("i=block/source");
        double avWriteTime @unit(s) = default(2s);	//2 s average
        double avObjectSize = default(1024);	//1 KB average
        double wait_time @unit(s);
        double generation_time @unit(s);
}

module GameModule like ITier
{
    gates:
        input from_lowerTier;    // gate from the lower tier
        input from_upperTier;    // gate from the upper tier
        input udpIn;    // gate from the UDP layer
        input trace_in;    // gate for trace file commands
        input tcpIn;             // gate from the TCP layer
        output tcpOut;           // gate to the TCP layer
        output to_lowerTier;    // gate to the lower tier
        output to_upperTier;    // gate to the upper tier
        output udpOut;    // gate to the UDP layer
    submodules:
        game: Game;
    connections allowunconnected:
        from_lowerTier --> game.from_lowerTier;
        to_lowerTier <-- game.to_lowerTier;
        trace_in --> game.trace_in;
}

simple Storage
{
    parameters:
        @class(Storage);
        @display("i=abstract/cache");
        @signal[qlen](type="int");
        @signal[qsize](type="int");
        @signal[queueingTime](type="simtime_t");
        @statistic[qlen](title="queue length"; record=vector,timeavg,max; interpolationmode=sample-hold);
        @statistic[qsize](title="queue size"; record=vector,timeavg,max; interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue"; unit=s; interpolationmode=none);
        @statistic[storeTime](title="time taken to store an object"; record=histogram,vector,timeavg; interpolationmode=sample-hold);

        @signal[storeTime](type="simtime_t");
        @statistic[storeTime](title="time taken to store a root object"; record=histogram,vector,timeavg; interpolationmode=sample-hold);

        @signal[Object](type="int");
        @statistic[Object](title="Objects"; record=sum);
    gates:
        output read;
        input write;
}

simple GroupStorage
{
    parameters:
        @class(GroupStorage);
        @display("i=abstract/cache");
        
        @signal[JoinTime](type="simtime_t");
        @statistic[JoinTime](title="group join time"; record=last);
        
        @signal[qlen](type="int");
        @signal[qsize](type="int");
        @signal[queueingTime](type="simtime_t");
        @statistic[qlen](title="queue length"; record=vector,timeavg,max; interpolationmode=sample-hold);
        @statistic[qsize](title="queue size"; record=vector,timeavg,max; interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue"; unit=s; interpolationmode=none);
        @statistic[storeTime](title="time taken to store an object"; record=histogram,vector,timeavg; interpolationmode=sample-hold);

        @signal[storeTime](type="simtime_t");
        @statistic[storeTime](title="time taken to store a root object"; record=histogram,vector,timeavg; interpolationmode=sample-hold);

        @signal[Object](type="int");
        @statistic[Object](title="Objects"; record=sum);
        
        @signal[GroupSize](type="int");
        @signal[GroupSendFail](type="int");
        
        @statistic[GroupSize](title="Group size"; record=last);
        @statistic[GroupSendFail](title="Group send fails"; record=sum);
        
        string directory_ip;
        int directory_port;
        double requestTimeout @unit(s);
        int replicas;
    gates:
        inout comms_gate;
        
        output read;
        input write;
        
        output to_upperTier;     // gate to the upper tier
        input from_upperTier;    // gate from the upper tier
}

simple OverlayStorage
{
    parameters:
        @class(OverlayStorage);
        @display("i=abstract/cache");
        
        int replicas = 1;
    gates:
        inout comms_gate;
        
        output read;
        input write;
}

simple GroupLedger
{
    parameters:
        @class(GroupLedger);
}

simple Peer_logic
{
    parameters:
        @class(Peer_logic);
        @display("i=block/cogwheel");
        
        int replicas;
    gates:
        inout comms_gate;
        
        output write;
        input read;
        output group_write;
        input group_read;
        output overlay_write;
        input overlay_read;
        
        input from_upperTier;    // gate from the upper tier
}

simple Super_peer_logic
{
    parameters:
        @class(Super_peer_logic);
        @display("i=block/cogwheel");
        double wait_time @unit(s);

        string directory_ip;
        int directory_port;
        
        bool objectReplication;

        @signal[OverlayWrite](type="int");
        @signal[GroupSize](type="int");
        @signal[OverlayDelivered](type="int");
        @signal[overlaysStoreFail](type="int");
        @signal[JoinTime](type="simtime_t");
        @signal[StoreNumber](type="int");
        @signal[OverlayNumber](type="int");

        @statistic[OverlayNumber](title="Number of overlay files in the group"; record=sum);
        @statistic[StoreNumber](title="Number of root/replica files in the group"; record=sum);
        @statistic[JoinTime](title="group join time"; record=vector);
        @statistic[OverlayWrite](title="Overlay writes"; record=sum);
        @statistic[GroupSize](title="Group size"; record=vector,max; interpolationmode=sample-hold);
        @statistic[OverlayDelivered](title="Delivered to Overlay"; record=sum);
        @statistic[overlaysStoreFail](title="Overlay store failures"; record=sum);
    gates:
        inout comms_gate;
        inout overlay_gate;
}

simple Communicator extends BaseApp
{
    parameters:
        @class(Communicator);
        @display("i=block/join");
    gates:
        inout gs_gate;
        inout os_gate;
        inout peer_gate;
        output toPeer_fromUpper;
        input fromPeer_toUpper;
        inout sp_group_gate @loose;
        inout sp_overlay_gate @loose;
}

simple Directory_logic extends BaseApp
{
    parameters:
        @class(Directory_logic);
        @display("i=block/cogwheel");

        @signal[SuperPeerNum](type="int");
        @signal[noSuperPeers](type="int");

        @statistic[SuperPeerNum](title="Number of Super Peers"; record=sum);
        @statistic[noSuperPeers](title="Number of times when no Super Peers were present"; record=sum);
}

module Directory like ITier
{
    parameters:
        @display("i=device/mainframe");
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
        directory_logic: Directory_logic;
    connections allowunconnected:
        //Connections to the Oversim simulation modules
        from_lowerTier --> directory_logic.from_lowerTier;
        to_lowerTier <-- directory_logic.to_lowerTier;
        udpIn --> directory_logic.udpIn;
        udpOut <-- directory_logic.udpOut;
        tcpIn --> directory_logic.tcpIn;
        tcpOut <-- directory_logic.tcpOut;
        trace_in --> directory_logic.trace_in;
}
