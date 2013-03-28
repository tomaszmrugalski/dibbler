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
			0xfa, 0xce, 0xb0, 0x0c,
			0, 7, 0, 9,
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

}
