#include <IPv6Addr.h>
#include <SrvCfgMgr.h>

#include <gtest/gtest.h>

using namespace std;

namespace {

    class SrvCfgMgrTest : public ::testing::Test {
    public:
        SrvCfgMgrTest() { }
    };

TEST_F(SrvCfgMgrTest, constructor) {

    string cfg="
iface \"eth0\" {
  class { pool 2001:db8:1111::/64 }

  option nis-server 2000::400,2000::401,2000::404,2000::405,2000::405
  option nis-domain nis.example.com
  option nis+-server 2000::501,2000::502
  option nis+-domain nisplus.example.com
}

iface vboxnet0 {
    class { pool 2003::/64 }
}";

    SPtr<TSrvCfgMgr> cfgmgr = new TSrvCfgMgr("testdata/server-1.conf", "testdata/server-CfgMgr1.xml");

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
        const List(TIPv6Addr) addrLst = opt->getAddrLst();

        ASSERT_EQ(5, addrLst);
        EXPECT_TRUE(string("2000::400"), addrLst[0]->getPlain());

        /// @todo: add validation of the remaining contents
    }
    

    delete cfgmgr;
}


}
