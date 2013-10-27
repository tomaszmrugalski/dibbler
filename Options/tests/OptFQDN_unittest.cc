#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "OptFQDN.h"

using namespace std;

namespace {

    // fully fualified domain name
const uint8_t expected1_len = 22;
const uint8_t expected1[] = { 0, OPTION_FQDN,
                           0, expected1_len,
                           0x7, // flags
                           0x8, 'h', 'o', 's', 't', 'n', 'a', 'm', 'e',
                           0x6, 'd', 'o', 'm', 'a', 'i', 'n',
                           0x3, 'o', 'r', 'g',
                           0x0 };

    // just a hostname (no trailing 0, so it does not end with a .)
const uint8_t expected2_len = 15;
const uint8_t expected2[] = { 0, OPTION_FQDN,
                              0, expected2_len,
                              0x5, // flags
                              13, 'j', 'u', 's', 't', 'a', 'h', 'o', 's', 't', 'n', 'a', 'm', 'e' };

    // empty option. (RFC4704, Section 4.2: A client MAY also leave the Domain
    // Name field empty if it desires the server to provide a name.
const uint8_t expected3_len = 1;
const uint8_t expected3[] = { 0, OPTION_FQDN,
                              0, expected3_len,
                              0x2 /* flags */ };

TEST(OptFQDNTest, parseBuildFullyQualified) {
    char buf[128];

    TOptFQDN* opt = new TOptFQDN((char*)expected1 + 4, (int)expected1_len, NULL);

    EXPECT_EQ(OPTION_FQDN, opt->getOptType() );

    EXPECT_EQ(expected1_len + 4u, opt->getSize());

    EXPECT_EQ(true, opt->isValid());

    EXPECT_EQ("hostname.domain.org", opt->getFQDN());
    EXPECT_EQ(true, opt->getNFlag()); // 7 = N + O + S bits
    EXPECT_EQ(true, opt->getOFlag());
    EXPECT_EQ(true, opt->getSFlag());

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + expected1_len + 4);

    EXPECT_FALSE( memcmp(buf, expected1, expected1_len + 4) );

    delete opt;
}

TEST(OptFQDNTest, parseBuildHostnameOnly) {
    char buf[128];

    TOptFQDN* opt = new TOptFQDN((char*)expected2 + 4, (int)expected2_len, NULL);

    EXPECT_EQ(OPTION_FQDN, opt->getOptType() );

    EXPECT_EQ(expected2_len + 4u, opt->getSize());

    EXPECT_EQ(true, opt->isValid());

    EXPECT_EQ("justahostname", opt->getFQDN());
    EXPECT_EQ(true, opt->getNFlag()); // 5 = N + S bits
    EXPECT_EQ(false, opt->getOFlag());
    EXPECT_EQ(true, opt->getSFlag());

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + expected2_len + 4);

    EXPECT_FALSE( memcmp(buf, expected2, expected2_len + 4) );

    delete opt;
}

TEST(OptFQDNTest, parseBuildEmpty) {
    char buf[128];

    TOptFQDN* opt = new TOptFQDN((char*)expected3 + 4, (int)expected3_len, NULL);

    EXPECT_EQ(OPTION_FQDN, opt->getOptType() );

    EXPECT_EQ(expected3_len + 4u, opt->getSize());

    EXPECT_EQ(true, opt->isValid());

    EXPECT_EQ("", opt->getFQDN());
    EXPECT_EQ(false, opt->getNFlag()); // 2 = O bit only
    EXPECT_EQ(true, opt->getOFlag());
    EXPECT_EQ(false, opt->getSFlag());

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + expected3_len + 4);

    EXPECT_FALSE( memcmp(buf, expected3, expected3_len + 4) );

    delete opt;
}

}
