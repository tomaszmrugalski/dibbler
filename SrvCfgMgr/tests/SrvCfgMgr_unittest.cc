#include "IPv6Addr.h"
#include "SrvCfgMgr.h"
#include "OptAddrLst.h"

#include <gtest/gtest.h>

using namespace std;

namespace {

    class SrvCfgMgrTest : public ::testing::Test {
    public:
        SrvCfgMgrTest() { }
    };

    class NakedSrvCfgMgr : public TSrvCfgMgr {
    public:
        NakedSrvCfgMgr(const std::string& config, const std::string& dbfile)
            :TSrvCfgMgr(config, dbfile) { }
    };

TEST_F(SrvCfgMgrTest, constructor) {

    string cfg = string("iface \"eth0\" {\n"
                        "  class { pool 2001:db8:1111::/64 }"
                        "  option nis-server 2000::400,2000::401,2000::404,2000::405,2000::405\n"
                        "  option nis-domain nis.example.com\n"
                        "  option nis+-server 2000::501,2000::502\n"
                        "  option nis+-domain nisplus.example.com\n"
                        "}\n"
                        "iface vboxnet0 {\n"
                        "  class { pool 2003::/64 }\n"
                        "}");

    SPtr<NakedSrvCfgMgr> cfgmgr = new NakedSrvCfgMgr("testdata/server-1.conf", "testdata/server-CfgMgr1.xml");

    SPtr<TSrvCfgIface> iface;
    cfgmgr->firstIface();
    while (iface = cfgmgr->getIface()) {
        if (iface->getName() == "eth0")
            break;
    }
    if (!iface) {
        ADD_FAILURE() << "Failed to find expected eth0 interface." << endl;
    } else {
        SPtr<TOptAddrLst> opt = (Ptr*)iface->getExtraOption(OPTION_NIS_SERVERS);
        EXPECT_TRUE(opt);
        List(TIPv6Addr) addrLst = opt->getAddrLst();

        ASSERT_EQ(5, addrLst.count());
        SPtr<TIPv6Addr> addr;
        addrLst.first();

        addr = addrLst.get();

        ASSERT_TRUE(addr);
        EXPECT_EQ(string("2000::400"), addr->getPlain());

        /// @todo: add validation of the remaining contents
    }
    

    delete cfgmgr;
}


}
