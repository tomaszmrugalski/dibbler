#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "OptIAAddress.h"

namespace {

 char expected[] = { 0, 5, // OPTION_IAADDR
                          0, 24, // length = 24
                          0x20, 0x1, 0x0d, 0xb8, 0, 1, 0, 0,
                          0, 0, 0, 0, 0xde, 0xad, 0xbe, 0xef,
                          0 , 0, 3, 0xe8,
                          0, 0, 7, 0xd0
};

TEST(OptIAAddressTest, storeSelf) {
    char buf[128];

    SPtr<TIPv6Addr> addr = new TIPv6Addr("2001:db8:1::dead:beef", true);
    TOptIAAddress* opt = new TOptIAAddress(addr, 1000, 2000, NULL);

    char* ptr = opt->storeSelf(buf);

    EXPECT_EQ(buf+28, ptr); // 28 - length of this option
    EXPECT_EQ(1000U, opt->getPref() );
    EXPECT_EQ(2000U, opt->getValid() );

    ASSERT_EQ(28, opt->getSize());

    ASSERT_FALSE( memcmp(buf, expected, 28) );

    delete opt;
}

TEST(OptIAAddressTest, parse) {
    char buf[128];

    char* ptr = expected+4;
    int len = 24;
    TOptIAAddress* opt = new TOptIAAddress(ptr, len, NULL);
    SPtr<TIPv6Addr> addr = opt->getAddr();

    EXPECT_EQ(string("2001:db8:1::dead:beef"), addr->getPlain());

    EXPECT_EQ(OPTION_IAADDR, opt->getOptType() );

    EXPECT_EQ( 1000, opt->getPref() );
    EXPECT_EQ( 2000, opt->getValid() );

    opt->storeSelf(buf);

    EXPECT_FALSE( memcmp(buf, expected, 20) );

    delete opt;
}


}
