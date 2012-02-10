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
import oversim.common.ITier_overlay;


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

module PeerSelectDHT like ITier_overlay
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

module Super_peerSelectDHT extends PeerSelectDHT
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
