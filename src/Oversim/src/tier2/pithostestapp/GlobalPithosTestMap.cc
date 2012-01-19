//
// Copyright (C) 2008 Institut fuer Telematik, Universitaet Karlsruhe (TH)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @file GlobalPithosTestMap.cc
 * @author Ingmar Baumgart
 */

#include <omnetpp.h>

#include <GlobalStatisticsAccess.h>

#include <DHTTestAppMessages_m.h>

#include "GlobalPithosTestMap.h"

using namespace std;

Define_Module(GlobalPithosTestMap);

GlobalPithosTestMap::GlobalPithosTestMap()
{
    periodicTimer = NULL;
}

GlobalPithosTestMap::~GlobalPithosTestMap()
{
    cancelAndDelete(periodicTimer);
    dataMap.clear();
    groupMap.clear();
}

void GlobalPithosTestMap::initialize()
{
    globalStatistics = GlobalStatisticsAccess().get();
    WATCH_MAP(dataMap);

    periodicTimer = new cMessage("PithosTestMapTimer");

    scheduleAt(simTime(), periodicTimer);
}

void GlobalPithosTestMap::finish()
{
}

void GlobalPithosTestMap::handleMessage(cMessage* msg)
{
    //cleanupDataMap();
    DhtTestEntryTimer *entryTimer = NULL;

    if (msg == periodicTimer)
    {
        RECORD_STATS(globalStatistics->recordOutVector(
           "GlobalPithosTestMap: Number of stored Pithos entries", dataMap.size()));
        scheduleAt(simTime() + TEST_MAP_INTERVAL, msg);

    } else if ((entryTimer = dynamic_cast<DhtTestEntryTimer*>(msg)) != NULL)
    {
    	eraseEntry(entryTimer->getKey());
        delete msg;

    } else {
        throw cRuntimeError("GlobalPithosTestMap::handleMessage(): "
                                "Unknown message type!");
    }
}

void GlobalPithosTestMap::insertEntry(const OverlayKey& key, const GameObject& entry)
{
    Enter_Method_Silent();

    //Insert the entry into the groupMap
    std::map<TransportAddress, std::vector<GameObject> >::iterator it;

    //std::cout << "Inserted new object with group address: " << entry.getGroupAddress() << endl;

    it = groupMap.find(entry.getGroupAddress());
    if (it == groupMap.end())
    {
    	std::vector<GameObject> object_list;
    	object_list.push_back(entry);
    	groupMap.insert(std::make_pair(entry.getGroupAddress(), object_list));

    } else {
    	it->second.push_back(entry);
    }

    //Insert the entry into the key map
    dataMap.erase(key);
    dataMap.insert(make_pair(key, entry));

    DhtTestEntryTimer* msg = new DhtTestEntryTimer("dhtEntryTimer");
    msg->setKey(key);

    scheduleAt(entry.getCreationTime()+entry.getTTL(), msg);
}

void GlobalPithosTestMap::eraseEntry(const OverlayKey& key)
{
	std::map<OverlayKey, GameObject>::iterator key_it;
	std::map<TransportAddress, std::vector<GameObject> >::iterator group_it;
	std::vector<GameObject>::iterator object_it;
	TransportAddress group_address;

	key_it = dataMap.find(key);
	if (key_it == dataMap.end())
		error("[GlobalPithosTestMap] Key not found in key map.");

	group_address = key_it->second.getGroupAddress();
	if (group_address.isUnspecified())
		error("[GlobalPithosTestMap] Group address is unspecified when erasing.");

	group_it = groupMap.find(key_it->second.getGroupAddress());
	if (group_it == groupMap.end())
		error("[GlobalPithosTestMap] Could not resolve super peer address for given overlay key.");

	for (object_it = (group_it->second).begin() ; object_it != (group_it->second).end() ; object_it++)
	{
		if (*object_it == key_it->second)
		{
			group_it->second.erase(object_it);
			break;
		}
	}

    dataMap.erase(key);
}

const GameObject* GlobalPithosTestMap::findEntry(const OverlayKey& key)
{
    std::map<OverlayKey, GameObject>::iterator it = dataMap.find(key);

    if (it == dataMap.end()) {
        return NULL;
    } else {
        return &(it->second);
    }
}

OverlayKey GlobalPithosTestMap::getRandomGroupKey(TransportAddress group_address)
{
	std::map<TransportAddress, std::vector<GameObject> >::iterator it;
	GameObject object;

	it = groupMap.find(group_address);
	if (it == groupMap.end())
	{
		//TODO: Log that a group could not be found
		return OverlayKey::UNSPECIFIED_KEY;
	}

	object = (it->second).at(intuniform(0, it->second.size()-1));

	return object.getHash();
}

const OverlayKey& GlobalPithosTestMap::getRandomKey()
{
    if (dataMap.size() == 0) {
        return OverlayKey::UNSPECIFIED_KEY;
    }

    // return random OverlayKey in O(log n)
    std::map<OverlayKey, GameObject>::iterator it = dataMap.end();
    GameObject tempEntry;

    OverlayKey randomKey = OverlayKey::random();
    it = dataMap.find(randomKey);

    if (it == dataMap.end()) {
        it = dataMap.insert(make_pair(randomKey, tempEntry)).first;
        dataMap.erase(it++);
    }

    if (it == dataMap.end()) {
        it = dataMap.begin();
    }

    return it->first;
}


