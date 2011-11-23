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
#include "GameObject.h"
#include "CommonMessages_m.h"
}}

class BaseCallMessage;
class BaseResponseMessage;
class GameObject;

class noncobject OverlayKey;
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
    GameObject go; // value that has to be stored on keys root
    int ttl; // time to life
    bool isModifiable; // true, if the data item may be overwritten by other nodes
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
}

//
// Message type to respond to a DHTget request @see DHTget
//
// @author Ingmar Baumgart
//
packet RootObjectGetCAPIResponse extends BaseResponseMessage
{
    DhtDumpEntry result[]; 
    bool isSuccess;
}