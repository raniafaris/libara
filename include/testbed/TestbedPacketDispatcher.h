/*
 * $FU-Copyright$
 */

#ifndef PACKET_DISPATCHER_H
#define PACKET_DISPATCHER_H

#include "CLibs.h"
#include "Testbed.h"
#include "Packet.h"
#include "testbed/TestbedNetworkInterface.h"
#include "TestbedARAClient.h"
#include "TestbedPacketFactory.h"
#include <unordered_map>

TESTBED_NAMESPACE_BEGIN

/**
 * The first ara callback of the mesh processing pipeline, determines if a packet belongs to libARA and sends to araMessageDispatcher, or the kernel, accordingly.
 */
dessert_cb_result toSys(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_meshif_t* interface, dessert_frameid_t id);

/**
 * Receives a Packet from an Interface and converts to a message, then sends it over selected interface (if NULL sends over all interfaces)
 */
int dispatch(const Packet* packet, std::shared_ptr<TestbedAddress> interfaceAddress, std::shared_ptr<Address> recipient);

/**
 *
 */
dessert_cb_result toMesh(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_sysif_t *interface, dessert_frameid_t id);

/**
 *
 */
void dumpDessertMessage(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags);

/**
 *
 */
dessert_cb_result packetFilter(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_sysif_t *interface, dessert_frameid_t id);

/**
 *
 */
std::string toString(dessert_msg_t* message, bool isNetworkByteOrder);

/**
 * The function is based on libdesserts 'dessert_msg_trace_dump'. However,
 * the function returns a string and has a slightly different behavior in
 * terms of error codes
 *
 * @param message The dessert message to scan for extension
 * @param type The type of extension which should be scanned for
 *
 * @return On success a string representation of the extension
 */
std::string toStringTrace(const dessert_msg_t* message, uint8_t type);

TESTBED_NAMESPACE_END

#endif /*PACKET_DISPATCHER_H*/
