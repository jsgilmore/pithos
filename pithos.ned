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

package pithos;

simple Storage
{
    parameters:
        @class(Storage);
        @display("i=abstract/cache");
        @signal[qlen](type="int");
        @signal[queueingTime](type="simtime_t");
        @statistic[qlen](title="queue length";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";unit=s;interpolationmode=none);
        
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
        
    gates:
        output out[];
        input in[];
        output write;
        input read;
        input request;
}

simple Super_peer_logic
{
    parameters:
        @class(Super_peer_logic);
        @display("i=block/cogwheel");
        @signal[busy](type="bool");
        @statistic[busy](title="server busy state";record=vector,timeavg;interpolationmode=sample-hold);
        
    gates:
        output out[];
        input in[];
        output write;
        input read;
        input request;
}

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

module Super_peer
{
    parameters:
        @display("i=device/pc_l");
    gates:
        input in[];
        output out[];
    submodules:
        super_peer_logic: Super_peer_logic;
        storage: Storage;
        game: Game;
    connections:
        game.write --> super_peer_logic.request;
        super_peer_logic.write --> storage.write;
        storage.read --> super_peer_logic.read;
        for i=0..sizeof(out)-1 {
            super_peer_logic.out++ --> out[i];
            in[i] --> super_peer_logic.in++;
        }
}

network Pithos
{
    @display("bgl=2");
    submodules:
        peer[20]: Peer;
        super_peer: Super_peer;
    connections:
        for j=0..18, for i=j..18 {
            peer[j].out++ --> {  datarate = 384Kbps; } --> peer[i+1].in++;
            peer[i+1].out++ --> {  datarate = 384Kbps; } --> peer[j].in++;
        }
        
        for i=0..19 {
            super_peer.out++ --> {  datarate = 384Kbps; } --> peer[i].in++;
            peer[i].out++ --> {  datarate = 384Kbps; } --> super_peer.in++;
        }
}
