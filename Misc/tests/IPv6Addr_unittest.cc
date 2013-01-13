#include "IPv6Addr.h"

#include <gtest/gtest.h>

namespace {

TEST(IPv6AddrTest, constructor) {

    SPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::abcd", true);
    
    /// @todo: implement tests for HostID class
}


}
