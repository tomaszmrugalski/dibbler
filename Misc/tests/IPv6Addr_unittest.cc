#include "IPv6Addr.h"

#include <string>
#include <gtest/gtest.h>

using namespace std;

namespace {

TEST(IPv6AddrTest, constructor) {

    SPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::abcd", true);

    EXPECT_EQ(string(addr->getPlain()), string("fe80::abcd"));
    EXPECT_TRUE(addr->linkLocal());
    // EXPECT_FALSE(addr->isLopback());
}

TEST(IPv6AddrTest, subtraction) {
    TIPv6Addr a("2001:db8:1::ffff:ffff:ffff:ffff", true);
    TIPv6Addr b("2001:db8:1::", true);

    TIPv6Addr result = a - b;

    EXPECT_EQ(string(result.getPlain()), "::ffff:ffff:ffff:ffff");

    // This is somewhat ill defined, but let's check it anyway
    result = b - a;
    EXPECT_EQ(string(result.getPlain()), "ffff:ffff:ffff:ffff::1");
}

TEST(IPv6AddrTest, randomDecrease) {
    srandom(time(NULL));

    TIPv6Addr x("1::", true);
    for (int i=0; i < 30; ++i) {
        TIPv6Addr y = x;
        --y;
        cout << "x=" << x.getPlain() << " y=" << y.getPlain() << endl;
        EXPECT_TRUE(y <= x);
    }
}

// Tests if truncation operation is conducted properly.
TEST(IPv6AddrTest, truncate) {
    TIPv6Addr x("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", true);

    x.truncate(0, 128);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", string(x.getPlain()));

    x.truncate(0, 127);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:fffe", string(x.getPlain()));

    x.truncate(0, 126);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:fffc", string(x.getPlain()));

    x.truncate(0, 125);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:fff8", string(x.getPlain()));

    x.truncate(0, 124);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:fff0", string(x.getPlain()));

    x.truncate(0, 120);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00", string(x.getPlain()));

    x.truncate(0, 112);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:0", string(x.getPlain()));

    x.truncate(0, 96);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff::", string(x.getPlain()));

    x.truncate(0, 64);
    EXPECT_EQ("ffff:ffff:ffff:ffff::", string(x.getPlain()));

    x.truncate(0, 8);
    EXPECT_EQ("ff00::", string(x.getPlain()));
}

}
