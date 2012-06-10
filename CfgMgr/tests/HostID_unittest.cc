#include "IPv6Addr.h"
#include "DUID.h"
#include "HostID.h"

#include <gtest/gtest.h>

namespace {

TEST(HostIDTest, constructor) {

    SPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::abcd", true);
    SPtr<TDUID> duid = new TDUID("00:01:02:03:04");
    
    SPtr<THostID> host1 = new THostID(addr);

    /// @todo: implement tests for HostID class
}


}
