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

}
