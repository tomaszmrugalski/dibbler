#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "OptVendorData.h"

using namespace std;

namespace {

const char expected[] = { 0, OPTION_REMOTE_ID,
                          0, 13, // length = 18 bytes
			  1, 2, 3, 4,
			  'r', 'e', 'm', 'o', 't', 'e', '-', 'i', 'd' };

TEST(OptVendorDataTest, parse) {
    char buf[128];

    TOptVendorData* opt = new TOptVendorData(OPTION_REMOTE_ID, expected+4, 13, NULL);

    EXPECT_EQ(OPTION_REMOTE_ID, opt->getOptType() );

    EXPECT_EQ(17u, opt->getSize());

    EXPECT_EQ(true, opt->isValid());

    EXPECT_EQ(0x1020304, opt->getVendor());

    ASSERT_EQ(9, opt->getVendorDataLen());

    EXPECT_EQ(0, memcmp("remote-id", opt->getVendorData(), 9));

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + 17);

    EXPECT_FALSE( memcmp(buf, expected, 17) );

    delete opt;
}

}
