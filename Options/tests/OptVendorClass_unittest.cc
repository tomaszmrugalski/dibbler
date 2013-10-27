#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "OptVendorClass.h"

using namespace std;

namespace {

const char expected[] = { 0, OPTION_VENDOR_CLASS,
                          0, 25,
			  1, 2, 3, 4,
                          0, 0x9, 'd', 'o', 'c', 's', 'i', 's', '3', '.', '0',
                          0, 0x2, 'i', 's',
                          0, 0x4, 'h', 'a', 'r', 'd' };

TEST(OptVendorClassTest, parse) {
    char buf[128];

    TOptVendorClass* opt = new TOptVendorClass(OPTION_VENDOR_CLASS, expected+4, 25, NULL);

    EXPECT_EQ(OPTION_VENDOR_CLASS, opt->getOptType() );

    EXPECT_EQ(29u, opt->getSize());

    EXPECT_EQ(true, opt->isValid());

    EXPECT_EQ(0x1020304u, opt->Enterprise_id_);

    ASSERT_EQ(3u, opt->userClassData_.size());

    ASSERT_EQ(9u, opt->userClassData_[0].opaqueData_.size());
    EXPECT_EQ(0, memcmp("docsis3.0", &opt->userClassData_[0].opaqueData_[0], 9));

    ASSERT_EQ(2u, opt->userClassData_[1].opaqueData_.size());
    EXPECT_EQ(0, memcmp("is", &opt->userClassData_[1].opaqueData_[0], 2));

    ASSERT_EQ(4u, opt->userClassData_[2].opaqueData_.size());
    EXPECT_EQ(0, memcmp("hard", &opt->userClassData_[2].opaqueData_[0], 4));
	      

    // check that parsed option can be stored back
    char * ptr = opt->storeSelf(buf);

    EXPECT_EQ(ptr, buf + 29);

    EXPECT_FALSE( memcmp(buf, expected, 29) );

    delete opt;
}

}
