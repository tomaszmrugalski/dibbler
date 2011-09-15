#include <IPv6Addr.h>
#include <AddrAddr.h>

#include <gtest/gtest.h>

namespace {

    class AddrAddrTest : public ::testing::Test {
    public:
        AddrAddrTest() { }
    };

TEST_F(AddrAddrTest, constructor) {
    SPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::abcd", true);

    SPtr<TAddrAddr> addrAddr = new TAddrAddr(addr, 100, 200);

    EXPECT_EQ(100u, addrAddr->getPref() );
    EXPECT_EQ(200u, addrAddr->getValid() );
}


}
