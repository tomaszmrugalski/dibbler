#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "OptVendorSpecInfo.h"

using namespace std;

namespace {


TEST(OptVendorSpecInfo, parse) {
    char buf[128];
    char buf2[128];

    char expected[] = { 0, OPTION_REMOTE_ID,
			0, 17, // length = 18 bytes
			0xfa, 0xce, 0xb0, 0x0c, // enterprise-id
			0, 7, // sub-option 7
                        0, 9, // sub-option length 9
			'r', 'e', 'm', 'o', 't', 'e', '-', 'i', 'd' };

    TOptVendorSpecInfo* opt = new TOptVendorSpecInfo(OPTION_REMOTE_ID, expected+4, 17, NULL);

    EXPECT_EQ(OPTION_REMOTE_ID, opt->getOptType() );

    EXPECT_EQ(21u, opt->getSize());

    EXPECT_EQ(true, opt->isValid());

    EXPECT_EQ(0xfaceb00c, opt->getVendor());

    SPtr<TOpt> sub_opt = opt->getOption(7);
    ASSERT_TRUE(sub_opt);

    ASSERT_EQ(9u, sub_opt->getSize() - 4);

    sub_opt->storeSelf(buf2);

    EXPECT_EQ(0, memcmp(buf2, expected + 8, 13));

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + 21);

    EXPECT_FALSE( memcmp(buf, expected, 21) );

    delete opt;
}

TEST(OptionVendorSpecInfo, string) {
    char expected[] = { 0, OPTION_REMOTE_ID,
			0, 17, // length = 18 bytes
			0xfa, 0xce, 0xb0, 0x0c, // enterprise-id
			0, 7, // sub-option 7
                        0, 9, // sub-option length 9
			'r', 'e', 'm', 'o', 't', 'e', '-', 'i', 'd' };
    const int expected_len = sizeof(expected);

    TOptVendorSpecInfo vendor(OPTION_REMOTE_ID, 0xfaceb00c, 7, string("remote-id"), NULL);
    char buf[128];
    char* ptr = vendor.storeSelf(buf);

    EXPECT_EQ(ptr, buf + expected_len);

    EXPECT_EQ(0, memcmp(expected, buf, expected_len));
}

// This test checks that the constructor that takes IPv6 address as a parameter
// is working properly
TEST(OptionVendorSpecInfo, addr) {

    char expected[] = { 0x01, 0x23, // option type = 0x123
			0, 24, // length = 24 bytes
			0x01, 0x23, 0x45, 0x67, // enterprise-id = 0x1234567
			0x89, 0x01, // sub-option 0x8901
                        0, 16, // sub-option length 16
                        0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, // IPv6 addr: 2001:db8::1
                        0, 0, 0, 0, 0, 0, 0, 1 };
    const int expected_len = sizeof(expected);

    SPtr<TIPv6Addr> addr(new TIPv6Addr("2001:db8::1", true));
    TOptVendorSpecInfo vendor(0x123, 0x01234567, 0x8901, addr, NULL);

    char buf[128];
    char* ptr = vendor.storeSelf(buf);
    EXPECT_EQ(ptr, buf + expected_len);

    EXPECT_EQ(0, memcmp(expected, buf, expected_len));
}

}
