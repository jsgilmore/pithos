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
    gates:
        output out[];
        input q_in;
        input write;
        output read @loose;
}

simple Queue
{
    parameters:
        @class(Queue);
        @display("i=block/queue;q=queue");
    gates:
        input in[];
        output out;
}

module Peer
{
    parameters:
        @display("i=device/pc");
    gates:
        input in[];
        output out[];
        input write @directIn;
    submodules:
        storage: Storage;
        queue: Queue;
    connections:
        queue.out --> storage.q_in;
        write --> storage.write;
        for i=0..sizeof(out)-1 {
            storage.out++ --> out[i];
            in[i] --> queue.in++;
        }
}

simple Game
{
    parameters:
        @class(Game);
        @display("i=block/source");
        double avWriteTime @unit(s) = default(5s);	//0.1 ms average
        double avObjectSize = default(5*1024*1024);	//5 MB average
}

network Pithos
{
    @display("bgl=2");
    submodules:
        peer[20]: Peer;
        super_peer: Peer;
        game: Game;
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
