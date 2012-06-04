#include "IPv6Addr.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"
#include "OptDUID.h"
#include "OptAddrLst.h"
#include "SrvMsgSolicit.h"
#include "SrvMsgAdvertise.h"

#include <gtest/gtest.h>

using namespace std;

namespace {

    class NakedSrvIfaceMgr: public TSrvIfaceMgr {
        NakedSrvIfaceMgr(const std::string& xmlFile)
            : TSrvIfaceMgr(xmlFile) {
            TSrvIfaceMgr::Instance = this;
        }
        ~NakedSrvIfaceMgr() {
            TSrvIfaceMgr::Instance = NULL;
        }
    };

    class NakedSrvAddrMgr: public TSrvAddrMgr {
        NakedSrvAddrMgr(const std::string& config, bool load_db)
            :TSrvAddrMgr(config, load_db) {
            TSrvAddrMgr::Instance = this;
        }
        ~NakedSrvAddrMgr() {
            TSrvAddrMgr::Instance = NULL;
        }
    };

    class NakedSrvCfgMgr : public TSrvCfgMgr {
    public:
        NakedSrvCfgMgr(const std::string& config, const std::string& dbfile)
            :TSrvCfgMgr(config, dbfile) {
            TSrvCfgMgr::Instance = this;
        }
        ~NakedSrvCfgMgr() {
            TSrvCfgMgr::Instance = NULL;
        }
    };
    class NakedSrvTransMgr: public TSrvTransMgr {
    public:
        NakedSrvTransMgr(const std::string& xmlFile): TSrvTransMgr(xmlFile) {
            TSrvTransMgr::Instance = this;
        }
        List(TSrvMsg)& getMsgLst() { return MsgLst; }
        ~NakedSrvTransMgr() {
            TSrvTransMgr::Instance =  NULL;
        }
    };

    class ServerTest : public ::testing::Test {
    public:
        ServerTest() {
            clntDuid = new TDUID("00:01:00:06:15:0e:50:52:dc:2b:61:e5:40:9c");
            clntId = new TOptDUID(OPTION_CLIENTID, clntDuid, NULL);
        }
        SPtr<TDUID> clntDuid;
        SPtr<TOptDUID> clntId;
    };


TEST_F(ServerTest, constructor) {

    TSrvIfaceMgr::instanceCreate("testdata/server-IfaceMgr1.xml");
    TIfaceMgr & ifacemgr = SrvIfaceMgr();

    ifacemgr.firstIface();
    SPtr<TIfaceIface> iface = ifacemgr.getIface();
    string iface_name = iface->getName();

    string cfg = string("iface \"" + iface_name + "\" {\n"
                        "  class { pool 2001:db8:1111::/64 }\n"
                        "  option nis-server 2000::400,2000::401,2000::404,2000::405,2000::405\n"
                        "  option nis-domain nis.example.com\n"
                        "  option nis+-server 2000::501,2000::502\n"
                        "  option nis+-domain nisplus.example.com\n"
                        "}\n");

    ofstream cfgfile("testdata/server-1.conf");
    cfgfile << cfg;
    cfgfile.close();

    SPtr<NakedSrvCfgMgr> cfgmgr = new NakedSrvCfgMgr("testdata/server-1.conf", "testdata/server-CfgMgr1.xml");

    TSrvAddrMgr::instanceCreate("testdata/server-AddrMgr1.xml", false); // don't load db

    NakedSrvTransMgr * transmgr = new NakedSrvTransMgr("testdata/server-TransMgr1.xml");

    // now generate client's message
    char empty[] = { 0x01, 0x2, 0x3, 0x4};
    SPtr<TIPv6Addr> clientAddr = new TIPv6Addr("fe80::1234", true);
    SPtr<TSrvMsgSolicit> sol = new TSrvMsgSolicit(iface->getID(), clientAddr, empty, sizeof(empty));
    sol->addOption((Ptr*)clntId);

    EXPECT_EQ(0, transmgr->getMsgLst().count());

    // process it through server usual routines
    transmgr->relayMsg((Ptr*)sol);

    EXPECT_EQ(1, transmgr->getMsgLst().count());

    List(TSrvMsg) & msglst = transmgr->getMsgLst();
    msglst.first();
    SPtr<TSrvMsg> x = msglst.get();
    ASSERT_TRUE(x);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)x;


    SPtr<TSrvCfgIface> cfgIface;
    cfgmgr->firstIface();
    while (cfgIface = cfgmgr->getIface()) {
        if (cfgIface->getName() == iface_name)
            break;
    }
    if (!cfgIface) {
        ADD_FAILURE() << "Failed to find expected " << iface_name << " interface." << endl;
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

        /// @todo: add validation of the remaining contents
    }


    //delete cfgmgr;
}


}
