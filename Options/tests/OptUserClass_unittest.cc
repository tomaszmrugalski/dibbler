#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "OptUserClass.h"

using namespace std;

namespace {

TEST(OptUserClassTest, parse) {
    char buf[128];

    const char expected[] = { 0, OPTION_USER_CLASS, // OPTION_UNICAST
			      0, 21, // length = 18 bytes
			      0, 0x9, 'd', 'o', 'c', 's', 'i', 's', '3', '.', '0',
			      0, 0x2, 'i', 's',
			      0, 0x4, 'h', 'a', 'r', 'd' };

    TOptUserClass* opt = new TOptUserClass(OPTION_USER_CLASS, expected+4, 21, NULL);

    EXPECT_EQ(OPTION_USER_CLASS, opt->getOptType() );

    EXPECT_EQ(25u, opt->getSize());

    EXPECT_EQ(true, opt->isValid());

    ASSERT_EQ(3u, opt->userClassData_.size());

    ASSERT_EQ(9u, opt->userClassData_[0].opaqueData_.size());
    EXPECT_EQ(0, memcmp("docsis3.0", &opt->userClassData_[0].opaqueData_[0], 9));

    ASSERT_EQ(2u, opt->userClassData_[1].opaqueData_.size());
    EXPECT_EQ(0, memcmp("is", &opt->userClassData_[1].opaqueData_[0], 2));

    ASSERT_EQ(4u, opt->userClassData_[2].opaqueData_.size());
    EXPECT_EQ(0, memcmp("hard", &opt->userClassData_[2].opaqueData_[0], 4));
	      

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + 25);

    EXPECT_FALSE( memcmp(buf, expected, 25) );

    delete opt;
}

}
