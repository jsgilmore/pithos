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

//
// TODO generated message class
//
packet PithosMsg {
    int value;
    int payloadType enum(PayloadTypes);
}

enum PayloadTypes
{
    WRITE = 1;		//A packet sent over the network, containing a game object to be stored.
    OVERLAY_WRITE = 2;
    INFORM = 3;		//A packet sent from a Super Peer, informing a peer of its ID.
};