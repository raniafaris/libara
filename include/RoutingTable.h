/*
 * $FU-Copyright$
 */

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_

#include "Packet.h"
#include "Address.h"
#include "NetworkInterface.h"
#include "EvaporationPolicy.h"
#include "RoutingTableEntry.h"
#include "LinearEvaporationPolicy.h"
#include "TimeFactory.h"

#include <deque>
#include <unordered_map>

namespace ARA {

class RoutingTable {

public:
    RoutingTable(TimeFactory* timeFactory);
    ~RoutingTable();

    float getPheromoneValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    void update(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface, float pheromoneValue);
    void removeEntry(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    std::deque<RoutingTableEntry*>* getPossibleNextHops(std::shared_ptr<Address> destination);
    std::deque<RoutingTableEntry*>* getPossibleNextHops(const Packet* packet);
    bool isDeliverable(std::shared_ptr<Address> destination);
    bool isDeliverable(const Packet* packet);

    bool exists(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

    void setEvaporationPolicy(EvaporationPolicy *policy);
    EvaporationPolicy *getEvaporationPolicy() const;
    std::unordered_map<std::shared_ptr<Address>, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate> getRoutingTable() const;
    void setRoutingTable(std::unordered_map<std::shared_ptr<Address>, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate> table);

protected:
    bool hasTableBeenAccessedEarlier();
    void triggerEvaporation();

    TimeFactory* timeFactory;
    Time *lastAccessTime;

private:
    std::unordered_map<std::shared_ptr<Address>, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate> table;
    /**
     * The memory management of the evaporationPolicy member is handled in class
     * ARA. Thus, there is no delete call to the evaporationPolicy member.
     */
    EvaporationPolicy *evaporationPolicy;
};

} /* namespace ARA */
#endif /* ROUTINGTABLE_H_ */
