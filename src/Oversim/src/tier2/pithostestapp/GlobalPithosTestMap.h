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
 * @file GlobalPithosTestMap.h
 * @author John Gilmore and Ingmar Baumgart
 */

#ifndef __GLOBAL_PITHOS_TEST_MAP_H__
#define __GLOBAL_PITHOS_TEST_MAP_H__

#include <map>

#include <omnetpp.h>

#include <OverlayKey.h>
#include <BinaryValue.h>

#include "GameObject.h"

class GlobalStatistics;


/**
 * Module with a global view on all currently stored Pithos records (used
 * by PithosTestApp).
 *
 * @author John Gilmore and Ingmar Baumgart
 */
class GlobalPithosTestMap : public cSimpleModule
{
public:
    GlobalPithosTestMap();
    ~GlobalPithosTestMap();

    /*
     * Insert a new key/value pair into global list of all currently
     * stored Pithos records.
     *
     * @param key The key of the record
     * @param entry The value and TTL of the record
     */
    void insertEntry(const OverlayKey& key, const GameObject& entry);

    /*
     * Returns the value and TTL for a given key from the global
     * list of all currently stored Pithos records.
     *
     * @param key The key of the record
     * @return The value and TTL of the record, NULL if no records was found
     */
    const GameObject* findEntry(const OverlayKey& key);

    /*
     * Erase the key/value pair with the given key from the global list of
     * all currently stored Pithos records.
     *
     * @param key The key of the record
     */
    void eraseEntry(const OverlayKey& key);

    OverlayKey getRandomGroupKey(TransportAddress group_address);

    OverlayKey getRandomNonGroupKey(TransportAddress group_address, int level = 0);

    /*
     * Returns the key of a random currently stored Pithos record from the global
     * list of all currently stored Pithos records.
     *
     * @return The key of the record, OverlayKey::UNSPECIFIED_KEY if the
     * global list is empty
     */
    const OverlayKey& getRandomKey();

    size_t size() { return dataMap.size(); };

private:
    void initialize();
    void handleMessage(cMessage* msg);
    void finish();

    static const int TEST_MAP_INTERVAL = 10; /**< interval in seconds for writing periodic statistical information */

    GlobalStatistics* globalStatistics; /**< pointer to GlobalStatistics module in this node */

    std::map<OverlayKey, GameObject> dataMap; /**< The map contains all currently stored Pithos records */

    std::map<TransportAddress, std::vector<GameObject> > groupMap; /**< The map contains all currently stored Pithos records, stored as vectors in a map sorted by group ID */

    cMessage *periodicTimer; /**< timer self-message for writing periodic statistical information */
};

#endif
