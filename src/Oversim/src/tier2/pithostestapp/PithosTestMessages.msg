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
#include "CommonMessages_m.h"
#include "GameObject.h"
#include "TransportAddress.h"
}}

class BaseCallMessage;
class BaseResponseMessage;
class GameObject;

class noncobject OverlayKey;
class noncobject TransportAddress;
class noncobject DhtDumpEntry;

//
// Message type to store value at the keys root
//
// @author Ingmar Baumgart
//
packet RootObjectPutCAPICall extends BaseCallMessage
{
    uint32_t kind = 1; // the kind of the stored data item
    uint32_t id = 1; // the id to identify multiple date items with same key and kind 
    int ttl; // time to life
    bool isModifiable; // true, if the data item may be overwritten by other nodes
    //Although not contained within this message, a GameObject is usually attached to the RPC call to be stored in Pithos.
}

//
// Message type to order the value corresponding to the OverlayKey key from the node responsible of key
//
// @author Ingmar Baumgart
//
packet RootObjectGetCAPICall extends BaseCallMessage
{
    OverlayKey key; // corresponding OverlayKey to the requested value
    uint32_t kind = 1; // the kind of the stored data item
    uint32_t id = 1; // the id to identify multiple date items with same key and kind 
}

//
// Message type to respond to a DHTput request @see DHTput
//
// @author Ingmar Baumgart
//
packet RootObjectPutCAPIResponse extends BaseResponseMessage
{
    bool isSuccess;
    TransportAddress groupAddress;
}

//
// Message type to respond to a DHTget request @see DHTget
//
// @author Ingmar Baumgart
//
packet RootObjectGetCAPIResponse extends BaseResponseMessage
{
    GameObject result; 
    bool isSuccess;
}