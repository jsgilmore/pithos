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

simple DHTStorage
{
    parameters:
        @class(DHTStorage);
        @display("i=abstract/cache");
    gates:
        inout comms_gate;
        
        output read;
        input write;
}

module Super_peerSelectDHT like ITier
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
        peer_logic: Peer_logic;
        super_peer_logic: Super_peer_logic;
        storage : Storage;
        group_storage: GroupStorage;
        dht_storage: DHTStorage;
        communicator: Communicator;
    connections allowunconnected:
        //Internal connection to the peer_logic, storage and game modules
        peer_logic.write --> storage.write;
        storage.read --> peer_logic.read;
        peer_logic.group_write --> group_storage.write;
        group_storage.read --> peer_logic.group_read;
        peer_logic.overlay_write --> dht_storage.write;
        dht_storage.read --> peer_logic.overlay_read;

        communicator.peer_gate <--> peer_logic.comms_gate;
        communicator.sp_group_gate <--> super_peer_logic.comms_gate;
        communicator.sp_overlay_gate <--> super_peer_logic.overlay_gate;
        communicator.toPeer_fromUpper --> peer_logic.from_upperTier;
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
        trace_in --> communicator.trace_in;
}
