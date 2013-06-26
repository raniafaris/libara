/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include <iostream>

#include "Testbed.h" 
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedAddressTest) {};

TEST(TestbedAddressTest, toString) {
    u_char macAddress[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    TestbedAddress address(macAddress);
    std::string expected = "1:2:3:4:5:6";

    STRCMP_EQUAL(expected.c_str(),address.toString().c_str());
}

TEST(TestbedAddressTest, getHashValue) {
    u_char macAddress[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    int leastSignificantBit = macAddress[5];
    int nextLeastSignificantBit = macAddress[4];
    TestbedAddress address(macAddress);
    int hashValueExpected = leastSignificantBit*256 + nextLeastSignificantBit;

    LONGS_EQUAL(hashValueExpected, address.getHashValue());
}

TEST(TestbedAddressTest, equals) {
    u_char macAddress[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    u_char sameMacAddress[ETHER_ADDR_LEN] = {1,2,3,4,5,6};

    TestbedAddress address(macAddress);
    TestbedAddress sameAddress(sameMacAddress);

    CHECK_TRUE(address == sameAddress);
}

TEST(TestbedAddressTest, notEquals) {
    u_char macAddress[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    u_char otherMacAddress[ETHER_ADDR_LEN] = {15,8,2,42,17,69};

    TestbedAddress address(macAddress);
    TestbedAddress otherAddress(otherMacAddress);

    CHECK_FALSE(address == otherAddress);
}

TESTBED_NAMESPACE_END