#include "IPv6Addr.h"
#include "DUID.h"
#include "HostRange.h"

#include <gtest/gtest.h>

namespace {

TEST(HostRangeTest, constructor) {
    SPtr<TIPv6Addr> addr1 = new TIPv6Addr("fe80::1", true);
    SPtr<TIPv6Addr> addr2 = new TIPv6Addr("fe80::ffff", true);

    SPtr<THostRange> range = new THostRange(addr1, addr2);

    /// @todo: implement tests for HostRange
}


}
