#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "OptAddr.h"
#include "OptRtPrefix.h"

namespace {

    char expected[] = {
        OPTION_NEXT_HOP/256, OPTION_NEXT_HOP%256, 0, 42,
        0x20, 0x01, 0x0d, 0xb8, 0xde, 0xad, 0xbe, 0xef,
        0, 0, 0, 0, 0, 0, 0, 1, // 2001:db8:dead:beef::1
        // ^--- NEXT-HOP, RT-PREFIX---v
        OPTION_RTPREFIX/256, OPTION_RTPREFIX%256, 0, 22,
        0, 0, 0x3, 0xe8, // lifetime = 1000
        64, 42, // prefix-length, metric
        0x20, 0x01, 0x0d, 0xb8, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0  // 2001:db8:1:: prefix
    };

// this test is for verifying that RTPREFIX can be stored in
// NextHop option
TEST(OptAddrTest, rtPrefixStoreSelf) {

    char buf[128];

    // rt-prefix option
    SPtr<TIPv6Addr> prefix = new TIPv6Addr("2001:db8:1::", true);
    SPtr<TOptRtPrefix> rtPrefix = new TOptRtPrefix(1000, // lifetime
                                                   64, // prefix length
                                                   42, // metric
                                                   prefix, // prefix
                                                   NULL);

    SPtr<TIPv6Addr> routerAddr = new TIPv6Addr("2001:db8:dead:beef::1", true);

    // next-hop option
    TOptAddr* nextHop = new TOptAddr(OPTION_NEXT_HOP, routerAddr, NULL);

    EXPECT_TRUE(rtPrefix->isValid());
    EXPECT_TRUE(nextHop->isValid());

    EXPECT_EQ(1000, rtPrefix->getLifetime());
    EXPECT_EQ(64, rtPrefix->getPrefixLen());
    EXPECT_EQ(42, rtPrefix->getMetric());

    // rt-rprefix on its own takes 26 bytes
    EXPECT_EQ(26, rtPrefix->getSize());

    // next-hop on its own takes 20 bytes
    EXPECT_EQ(20, nextHop->getSize());

    nextHop->addOption((Ptr*)rtPrefix);

    // together, they should take 46 bytes
    EXPECT_EQ(46, nextHop->getSize() );

    char* ptr = nextHop->storeSelf(buf);
    ASSERT_EQ(buf+46, ptr);

    EXPECT_EQ(0, memcmp(buf, expected, 46) );

    delete nextHop;
}

TEST(OptAddrTest, rtPrefixParse) {

    SPtr<TIPv6Addr> prefix = new TIPv6Addr("2001:db8:1::", true);
    SPtr<TIPv6Addr> routerAddr = new TIPv6Addr("2001:db8:dead:beef::1", true);

    SPtr<TOptAddr> nextHop = new TOptAddr(OPTION_NEXT_HOP, expected+4, 42, NULL);

    EXPECT_EQ(46, nextHop->getSize());
    EXPECT_EQ(OPTION_NEXT_HOP, nextHop->getOptType());
    EXPECT_TRUE(nextHop->isValid());
    SPtr<TOptRtPrefix> rtPrefix = (Ptr*)nextHop->getOption(OPTION_RTPREFIX);

    // there should be option OPTION_RTPREFIX
    ASSERT_TRUE(rtPrefix);
    EXPECT_TRUE(nextHop->isValid());

    EXPECT_EQ(1000, rtPrefix->getLifetime());
    EXPECT_EQ(64, rtPrefix->getPrefixLen());
    EXPECT_EQ(42, rtPrefix->getMetric());
}

}
