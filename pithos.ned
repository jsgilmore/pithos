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
        @statistic[qlen](title="queue length"; record=vector,timeavg,max; interpolationmode=sample-hold);
        @statistic[qsize](title="queue size"; record=vector,timeavg,max; interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue"; unit=s; interpolationmode=none);

        @signal[OverlayObject](type="int");
        @signal[RootObject](type="int");
        @signal[ReplicaObject](type="int");
        @statistic[OverlayObject](title="Overlay Objects"; record=sum);
        @statistic[RootObject](title="Root Objects"; record=sum);
        @statistic[ReplicaObject](title="Replica Objects"; record=sum);
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
        @statistic[busy](title="server busy state"; record=vector,timeavg; interpolationmode=sample-hold);
        int network_size = 21;
        int replicas_g = 3;
        int replicas_sp = 2;
    gates:
        inout comms_gate;
        output write;
        input read;
        input request;
}

simple Super_peer_logic
{
    parameters:
        @class(Super_peer_logic);
        @display("i=block/cogwheel");
        int network_size = 21;
        @signal[OverlayWrite](type="int");
        @statistic[OverlayWrite](title="Overlay writes"; record=sum);
        int largestKey;
    gates:
        inout comms_gate;
}

simple Communicator extends BaseApp
{
    parameters:
        @class(Communicator);
        @display("i=block/join");
        int sendPeriod @unit(s);   // how much time to wait between sending packets
        int numToSend;             // how many packets will we send at the same time?
        int largestKey;            // largest key we can pick
    gates:
        inout peer_gate;
        inout sp_gate @loose;
}

module Peer like ITier
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
    submodules:
        peer_logic: Peer_logic;
        storage: Storage;
        game: Game;
        communicator: Communicator;
    connections:
        game.write --> peer_logic.request;
        peer_logic.write --> storage.write;
        storage.read --> peer_logic.read;
        communicator.peer_gate <--> peer_logic.comms_gate;

        //Connections to the Oversim simulation modules
        from_lowerTier --> communicator.from_lowerTier;
        to_lowerTier <-- communicator.to_lowerTier;
        udpIn --> communicator.udpIn;
        udpOut <-- communicator.udpOut;
        tcpIn --> communicator.tcpIn;
        tcpOut <-- communicator.tcpOut;
        trace_in --> communicator.trace_in;
}

module Super_peer like ITier
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
    submodules:
        peer_logic: Peer_logic {
            @display("p=230,193");
        }
        super_peer_logic: Super_peer_logic;
        storage: Storage;
        game: Game;
        communicator: Communicator;
    connections allowunconnected:
        //Internal connection to the peer_logic, storage and game modules
        game.write --> peer_logic.request;
        peer_logic.write --> storage.write;
        storage.read --> peer_logic.read;

        communicator.peer_gate <--> peer_logic.comms_gate;
        communicator.sp_gate <--> super_peer_logic.comms_gate;

		//Connections to the Oversim simulation modules
        from_lowerTier --> communicator.from_lowerTier;
        to_lowerTier <-- communicator.to_lowerTier;
        udpIn --> communicator.udpIn;
        udpOut <-- communicator.udpOut;
        tcpIn --> communicator.tcpIn;
        tcpOut <-- communicator.tcpOut;
        trace_in --> communicator.trace_in;
}
