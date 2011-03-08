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
        @signal[qlen](type="int");
        @signal[busy](type="bool");
        @signal[queueingTime](type="simtime_t");
        @statistic[qlen](title="queue length";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[busy](title="server busy state";record=vector,timeavg;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";unit=s;interpolationmode=none);
        
    gates:
        output out[];
        input in[];
        input write;
        output read @loose;
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
        storage: Storage;
        game: Game;
    connections:
        game.write --> storage.write;
        for i=0..sizeof(out)-1 {
            storage.out++ --> out[i];
            in[i] --> storage.in++;
        }
}

network Pithos
{
    @display("bgl=2");
    submodules:
        peer[20]: Peer;
        super_peer: Peer;
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
