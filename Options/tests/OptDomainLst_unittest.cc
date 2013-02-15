#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "OptDomainLst.h"

using namespace std;

namespace {

const char expected[] = { 0, OPTION_NIS_DOMAIN_NAME, // OPTION_UNICAST
                          0, 18, // length = 18 bytes
                          0x4, 'a', 'f', 't', 'r',
                          0x7, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
                          0x3, 'c', 'o', 'm',
                          0x0 };

TEST(OptAddrTest, storeSelf1) {
    char buf[128];

    TOptDomainLst* opt = new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, "aftr.example.com", NULL);

    char* ptr = opt->storeSelf(buf);

    ASSERT_EQ(buf+22, ptr);

    ASSERT_EQ(22u, opt->getSize());

    ASSERT_FALSE( memcmp(buf, expected, 22) );

    delete opt;
}

TEST(OptAddrTest, parse1) {
    char buf[128];

    TOptDomainLst* opt = new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, expected+4, 18, NULL);

    EXPECT_EQ(string("aftr.example.com"), opt->getDomain());

    EXPECT_EQ(OPTION_NIS_DOMAIN_NAME, opt->getOptType() );

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + 22);

    EXPECT_FALSE( memcmp(buf, expected, 22) );

    delete opt;
}

}
