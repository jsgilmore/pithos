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

cplusplus {{
#include "packet_m.h"
#include "PeerData.h"
}}

class noncobject Packet;
class noncobject PeerData;

packet PeerListPkt extends Packet
{
    @customize(true);
    
    string objectName;		//This string is actually modelled as a long long int (64 bits) for the purposes of packet transmission, but a string is used to make the stored object easily readable
    unsigned long objectSize;
    abstract PeerData peer_list[];
}