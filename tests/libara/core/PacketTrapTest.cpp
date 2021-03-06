/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "EvaporationPolicy.h"
#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

#include <memory>
#include <deque>
#include <algorithm>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(PacketTrapTest) {
    ARAClientMock* client;
    PacketTrap* packetTrap;
    RoutingTable* routingTable;
    EvaporationPolicy* evaporationPolicy;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new ARAClientMock();
        packetTrap = client->getPacketTrap();
        routingTable = client->getRoutingTable();
        evaporationPolicy = routingTable->getEvaporationPolicy();
        interface = client->createNewNetworkInterfaceMock();
    }

    void teardown() {
        delete client;
    }
};

TEST(PacketTrapTest, trapPacket) {
    Packet* packet = new PacketMock();

    // Check that there is no trapped packet for the packets destination
    CHECK(packetTrap->contains(packet) == false);

    packetTrap->trapPacket(packet);

    // Now there must be a trapped packet for the packets destination
    CHECK(packetTrap->contains(packet) == true);
}

TEST(PacketTrapTest, trapMultiplePackets) {
    Packet* packet1 = new PacketMock("A", "B", 1);
    Packet* packet2 = new PacketMock("A", "B", 2);
    Packet* packet3 = new PacketMock("X", "Y", 1);
    Packet* packet4 = new PacketMock("A", "C", 3);

    // start the test
    CHECK(packetTrap->isEmpty());
    packetTrap->trapPacket(packet1);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == false);
    CHECK(packetTrap->contains(packet3) == false);
    CHECK(packetTrap->contains(packet4) == false);

    packetTrap->trapPacket(packet2);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == true);
    CHECK(packetTrap->contains(packet3) == false);
    CHECK(packetTrap->contains(packet4) == false);

    packetTrap->trapPacket(packet3);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == true);
    CHECK(packetTrap->contains(packet3) == true);
    CHECK(packetTrap->contains(packet4) == false);

    packetTrap->trapPacket(packet4);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == true);
    CHECK(packetTrap->contains(packet3) == true);
    CHECK(packetTrap->contains(packet4) == true);
}

TEST(PacketTrapTest, untrapDeliverablePackets) {
    Packet* trappedPacket1 = new PacketMock("src", "dest");
    Packet* trappedPacket2 = new PacketMock("src", "foo");
    AddressPtr destination1 = trappedPacket1->getDestination();
    AddressPtr destination2 = trappedPacket2->getDestination();
    AddressPtr someAddress (new AddressMock("bar"));

    // Start the test
    PacketQueue deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.empty());   // there is no trapped packet so none can be deliverable

    packetTrap->trapPacket(trappedPacket1);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.empty());   // packet is still not deliverable
    BYTES_EQUAL(1, packetTrap->getNumberOfTrappedPackets());

    // update the route to some other destination which should not make this destination deliverable
    routingTable->update(someAddress, someAddress, interface, 8);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.empty());
    BYTES_EQUAL(1, packetTrap->getNumberOfTrappedPackets());

    // trap another packet for a different destination
    packetTrap->trapPacket(trappedPacket2);
    BYTES_EQUAL(2, packetTrap->getNumberOfTrappedPackets());

    // now update the route to our wanted destination which should make it deliverable
    routingTable->update(trappedPacket1->getDestination(), someAddress, interface, 10);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.size() == 1);

    // the packet should have been removed from the packet trap
    BYTES_EQUAL(1, packetTrap->getNumberOfTrappedPackets());
    Packet* deliverablePacket = deliverablePackets.front();
    CHECK(deliverablePacket == trappedPacket1);
    delete deliverablePacket;

    // also get the other trapped packet
    routingTable->update(trappedPacket2->getDestination(), someAddress, interface, 10);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination2);
    CHECK(deliverablePackets.size() == 1);

    // the packet should have been removed from the packet trap
    BYTES_EQUAL(0, packetTrap->getNumberOfTrappedPackets());
    deliverablePacket = deliverablePackets.front();
    CHECK(deliverablePacket == trappedPacket2);
    delete deliverablePacket;
}

TEST(PacketTrapTest, testIsEmpty) {
    PacketMock packet = PacketMock();

    CHECK(packetTrap->isEmpty());

    packetTrap->trapPacket(&packet);
    CHECK(packetTrap->isEmpty() == false);

    // make the trapped packet deliverable
    AddressPtr destination = packet.getDestination();
    routingTable->update(destination, destination, interface, 10);

    packetTrap->untrapDeliverablePackets(destination);
    CHECK(packetTrap->isEmpty());
}

TEST(PacketTrapTest, deleteTrappedPacketsInDestructor) {
    Packet* packet = new PacketMock("source", "destination", 1);
    packetTrap->trapPacket(packet);

    // when the test finishes, the client will be deleted in teardown()
    // and the packet clone should be deleted as well
}

TEST(PacketTrapTest, removePacketsForDestination) {
    AddressPtr someAddress = AddressPtr(new AddressMock("foo"));
    Packet* packet1 = new PacketMock("A", "B", 1);
    Packet* packet2 = new PacketMock("A", "B", 2);
    Packet* packet3 = new PacketMock("X", "Y", 1);
    Packet* packet4 = new PacketMock("A", "C", 3);

    PacketQueue removedPackets = packetTrap->removePacketsForDestination(someAddress);
    CHECK(removedPackets.empty());

    packetTrap->trapPacket(packet1);
    packetTrap->trapPacket(packet2);
    packetTrap->trapPacket(packet3);
    packetTrap->trapPacket(packet4);

    removedPackets = packetTrap->removePacketsForDestination(packet1->getDestination());

    // check that packet 1 and 2 are in the returned list (damn std api suckz)
    BYTES_EQUAL(2, removedPackets.size());
    CHECK(std::find(removedPackets.begin(), removedPackets.end(), packet1) != removedPackets.end());
    CHECK(std::find(removedPackets.begin(), removedPackets.end(), packet2) != removedPackets.end());

    // check that the packets are no longer in the trap
    CHECK_FALSE(packetTrap->contains(packet1));
    CHECK_FALSE(packetTrap->contains(packet2));

    // check that the other packets are still in the trap
    CHECK_TRUE(packetTrap->contains(packet3));
    CHECK_TRUE(packetTrap->contains(packet4));

    delete packet1;
    delete packet2;
}

TEST(PacketTrapTest, packetOrderIsPreserved) {
    AddressPtr destination = AddressPtr(new AddressMock("dst"));
    Packet* packet1 = new PacketMock("src", "dst", 1);
    Packet* packet2 = new PacketMock("src", "foo", 2);
    Packet* packet3 = new PacketMock("src", "dst", 1);
    Packet* packet4 = new PacketMock("src", "foo", 3);
    Packet* packet5 = new PacketMock("src", "dst", 1);
    Packet* packet6 = new PacketMock("src", "dst", 1);
    Packet* packet7 = new PacketMock("src", "foo", 1);

    packetTrap->trapPacket(packet1);
    packetTrap->trapPacket(packet2);
    packetTrap->trapPacket(packet3);
    packetTrap->trapPacket(packet4);
    packetTrap->trapPacket(packet5);
    packetTrap->trapPacket(packet6);
    packetTrap->trapPacket(packet7);

    routingTable->update(destination, destination, interface, 10);
    PacketQueue trappedPackets = packetTrap->untrapDeliverablePackets(destination);
    BYTES_EQUAL(4, trappedPackets.size());
    CHECK(trappedPackets.at(0) == packet1);
    CHECK(trappedPackets.at(1) == packet3);
    CHECK(trappedPackets.at(2) == packet5);
    CHECK(trappedPackets.at(3) == packet6);

    delete packet1;
    delete packet3;
    delete packet5;
    delete packet6;
}
