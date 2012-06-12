#include "IPv6Addr.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "OptAddrLst.h"

#include <gtest/gtest.h>

using namespace std;

namespace {

    class NakedSrvIfaceMgr: public TSrvIfaceMgr {
    public:
        NakedSrvIfaceMgr(const std::string& xmlFile)
            : TSrvIfaceMgr(xmlFile) {
            TSrvIfaceMgr::Instance = this;
        }
        ~NakedSrvIfaceMgr() {
            TSrvIfaceMgr::Instance = NULL;
        }
    };

    class SrvCfgMgrTest : public ::testing::Test {
    public:
        SrvCfgMgrTest() {
            ifacemgr_ = new NakedSrvIfaceMgr("testdata/server-IfaceMgr.xml");

            // try to pick up an up and running interface
            ifacemgr_->firstIface();
            while ( (iface_ = ifacemgr_->getIface())
                    && (!iface_->flagUp() || !iface_->flagRunning())) {
            }
        }
        NakedSrvIfaceMgr * ifacemgr_;
        SPtr<TIfaceIface> iface_;
    };

    class NakedSrvCfgMgr : public TSrvCfgMgr {
    public:
        NakedSrvCfgMgr(const std::string& config, const std::string& dbfile)
            :TSrvCfgMgr(config, dbfile) { }
    };

TEST_F(SrvCfgMgrTest, constructor) {

    ASSERT_TRUE(iface_);
    string cfg = string("iface \"") + iface_->getName() + "\" {\n"
                        "  class { pool 2001:db8:1111::/64 }\n"
                        "  option nis-server 2000::400,2000::401,2000::404,2000::405,2000::405\n"
                        "  option nis-domain nis.example.com\n"
                        "  option nis+-server 2000::501,2000::502\n"
                        "  option nis+-domain nisplus.example.com\n"
                        "}\n"
                        "\n"
                        "iface nonexistent0 {\n"
                        "  class { pool 2003::/64 }\n"
                        "}";

    ofstream cfgfile("testdata/server-1.conf");
    cfgfile << cfg;
    cfgfile.close();

    SPtr<NakedSrvCfgMgr> cfgmgr = new NakedSrvCfgMgr("testdata/server-1.conf", "testdata/server-CfgMgr1.xml");

    SPtr<TSrvCfgIface> cfgIface;
    cfgmgr->firstIface();
    while (cfgIface = cfgmgr->getIface()) {
        if (cfgIface->getName() == iface_->getName())
            break;
    }
    if (!cfgIface) {
        ADD_FAILURE() << "Failed to find expected " << iface_->getName() << " interface." << endl;
    } else {
        SPtr<TOptAddrLst> opt = (Ptr*)cfgIface->getExtraOption(OPTION_NIS_SERVERS);
        EXPECT_TRUE(opt);
        List(TIPv6Addr) addrLst = opt->getAddrLst();

        ASSERT_EQ(5, addrLst.count());
        SPtr<TIPv6Addr> addr;
        addrLst.first();

        addr = addrLst.get();

        ASSERT_TRUE(addr);
        EXPECT_EQ(string("2000::400"), addr->getPlain());
    }


    //delete cfgmgr;
}


}
