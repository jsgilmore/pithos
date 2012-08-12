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
import oversim.common.ITier_overlay;

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
        
        string directory_ip;
        int directory_port;
        double requestTimeout @unit(s);
        int replicas;
        int numGetRequests;
        
        bool objectRepair;
        string repairType;
        bool gracefulMigration;
        double pingTime @unit(s);
        
        double malicious_peer_p;
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
        bool disableDHT;
        string putType;
        string getType;
        int numGetRequests;
        int numGetCompares;
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
        
        int replicas;
        
        bool objectRepair;
        string repairType;
        double repairTime;

        @signal[JoinTime](type="simtime_t");
        @statistic[JoinTime](title="group join time"; record=vector);
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

simple DHTStorage
{
    parameters:
        @class(DHTStorage);
        @display("i=abstract/cache");

        int testTtl;      // TTL for stored test records
        bool activeNetwInitPhase;    // send messages when network is in init phase?
    gates:

        inout comms_gate;
        output read;
        input write;
}

module Peer like ITier_overlay
{
    parameters:
        @display("i=device/pc");
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
        input overlayIn;         // gate from the Overlay layer
        output overlayOut;       // gate to the Overlay layer
    submodules:
        peer_logic: Peer_logic;
        group_storage: GroupStorage;
        group_ledger: GroupLedger;
        dht_storage: DHTStorage;
        communicator: Communicator;
    connections allowunconnected:
        peer_logic.group_write --> group_storage.write;
        peer_logic.group_read <-- group_storage.read;
        peer_logic.overlay_write --> dht_storage.write;
        peer_logic.overlay_read <-- dht_storage.read;
        
        communicator.peer_gate <--> peer_logic.comms_gate;
        communicator.toPeer_fromUpper --> group_storage.from_upperTier;
        communicator.fromPeer_toUpper <-- group_storage.to_upperTier;
        communicator.gs_gate <--> group_storage.comms_gate;
        communicator.os_gate <--> dht_storage.comms_gate;

        //Connections to the Oversim simulation modules
        from_lowerTier --> communicator.from_lowerTier;
        to_lowerTier <-- communicator.to_lowerTier;
        from_upperTier --> communicator.from_upperTier;
        to_upperTier <-- communicator.to_upperTier;
        udpIn --> communicator.udpIn;
        udpOut <-- communicator.udpOut;
        tcpIn --> communicator.tcpIn;
        tcpOut <-- communicator.tcpOut;
        overlayIn --> communicator.overlayIn;
        overlayOut <-- communicator.overlayOut;
        trace_in --> communicator.trace_in;
}

module Super_peer extends Peer
{
    parameters:
        @display("i=device/pc_l");
    submodules:
        super_peer_logic: Super_peer_logic;
        sp_group_ledger: GroupLedger;
    connections allowunconnected:
        communicator.sp_group_gate <--> super_peer_logic.comms_gate;
        communicator.sp_overlay_gate <--> super_peer_logic.overlay_gate;
}

module Super_peer_pure like ITier_overlay
{
    parameters:
        @display("i=device/pc_l");
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
        input overlayIn;         // gate from the Overlay layer
        output overlayOut;       // gate to the Overlay layer
    submodules:
        communicator: Communicator;
        super_peer_logic: Super_peer_logic;
        sp_group_ledger: GroupLedger;
    connections allowunconnected:
        communicator.sp_group_gate <--> super_peer_logic.comms_gate;
        communicator.sp_overlay_gate <--> super_peer_logic.overlay_gate;

        //Connections to the Oversim simulation modules
        from_lowerTier --> communicator.from_lowerTier;
        to_lowerTier <-- communicator.to_lowerTier;
        from_upperTier --> communicator.from_upperTier;
        to_upperTier <-- communicator.to_upperTier;
        udpIn --> communicator.udpIn;
        udpOut <-- communicator.udpOut;
        tcpIn --> communicator.tcpIn;
        tcpOut <-- communicator.tcpOut;
        overlayIn --> communicator.overlayIn;
        overlayOut <-- communicator.overlayOut;
        trace_in --> communicator.trace_in;
}
