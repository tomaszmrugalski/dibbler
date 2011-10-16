#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "OptAddr.h"

namespace {

const char expected[] = { 0, 12, // OPTION_UNICAST
                          0, 16, // length = 16 bytes (+4 hdr=20)
                          0x20, 0x1, 0x0d, 0xb8, 0, 1, 0, 0,
                          0, 0, 0, 0, 0xde, 0xad, 0xbe, 0xef };

TEST(OptAddrTest, storeSelf) {
    char buf[128];

    SPtr<TIPv6Addr> addr = new TIPv6Addr("2001:db8:1::dead:beef", true);
    TOptAddr* opt = new TOptAddr(OPTION_UNICAST, addr, NULL);

    char* ptr = opt->storeSelf(buf);

    ASSERT_EQ(buf+20, ptr);

    ASSERT_EQ(20, opt->getSize());

    ASSERT_FALSE( memcmp(buf, expected, 20) );

    delete opt;
}

TEST(OptAddrTest, parse) {
    char buf[128];

    TOptAddr* opt = new TOptAddr(OPTION_UNICAST, expected+4, 16, NULL);
    SPtr<TIPv6Addr> addr = opt->getAddr();

    EXPECT_EQ(string("2001:db8:1::dead:beef"), addr->getPlain());

    EXPECT_EQ(OPTION_UNICAST, opt->getOptType() );

    opt->storeSelf(buf);

    EXPECT_FALSE( memcmp(buf, expected, 20) );

    delete opt;
}


}
