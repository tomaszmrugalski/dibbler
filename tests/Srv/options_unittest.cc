#include "assign_utils.h"
#include <gtest/gtest.h>
#include "OptAddrLst.h"

using namespace std;

namespace test {

TEST_F(ServerTest, CfgMgr_options1) {

    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:1111::/64 }\n"
                 "  option nis-server 2000::400,2000::401,2000::404,2000::405,2000::406\n"
                 "  option nis-domain nis.example.com\n"
                 "  option nis+-server 2000::501,2000::502\n"
                 "  option nis+-domain nisplus.example.com\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // check that NIS-SERVERS option is handled properly
    SPtr<TOptAddrLst> opt = (Ptr*)cfgIface_->getExtraOption(OPTION_NIS_SERVERS);
    ASSERT_TRUE(opt); // check that NIS-servers are supported
    List(TIPv6Addr) addrLst = opt->getAddrLst();

    ASSERT_EQ(5u, addrLst.count());
    addrLst.first();

    EXPECT_EQ(string("2000::400"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::401"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::404"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::405"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::406"), addrLst.get()->getPlain());
    EXPECT_FALSE(addrLst.get()); // no additional addresses
}

}
