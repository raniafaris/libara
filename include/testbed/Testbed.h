/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_H_
#define _TESTBED_H_

#include "ARAMacros.h"

#include "CLibs.h"

#define TESTBED_NAMESPACE_BEGIN namespace ARA { namespace testbed {
#define TESTBED_NAMESPACE_END }}

TESTBED_NAMESPACE_BEGIN

typedef u_char ara_address_t[ETHER_ADDR_LEN];

#define DESSERT_LOCAL_ADDRESS dessert_l25_defsrc
#define DESSERT_BROADCAST_ADDRESS ether_broadcast

TESTBED_NAMESPACE_END

#endif 
