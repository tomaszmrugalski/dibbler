#include "IPv6Addr.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"
#include "OptDUID.h"
#include "OptAddrLst.h"
#include "SrvMsgSolicit.h"
#include "SrvMsgAdvertise.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"

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

    class NakedSrvAddrMgr: public TSrvAddrMgr {
    public:
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
            clntDuid_ = new TDUID("00:01:00:06:15:0e:50:52:dc:2b:61:e5:40:9c");
            clntId_ = new TOptDUID(OPTION_CLIENTID, clntDuid_, NULL);
            clntAddr_ = new TIPv6Addr("fe80::1234", true);

            ifacemgr_ = new NakedSrvIfaceMgr("testdata/server-IfaceMgr.xml");

            // try to pick up an up and running interface
            ifacemgr_->firstIface();
            while ( (iface_ = ifacemgr_->getIface()) && (!iface_->flagUp() || !iface_->flagRunning())) {
            }

        }

        bool createMgrs(string config) {

            if (!iface_) {
                ADD_FAILURE() << "No suitable interface detected: all are down or not running";
                return false;
            }

            // try to repalace IFACE name with an actual string name
            size_t pos = config.find("REPLACE_ME");
            if (pos != string::npos) {
                config.replace(pos, 10, iface_->getName());
            }

            ofstream cfgfile("testdata/server.conf");
            cfgfile << config;
            cfgfile.close();

            cfgmgr_ = new NakedSrvCfgMgr("testdata/server.conf", "testdata/server-CfgMgr.xml");
            addrmgr_ = new NakedSrvAddrMgr("testdata/server-AddrMgr.xml", false); // don't load db
            transmgr_ = new NakedSrvTransMgr("testdata/server-TransMgr.xml");


            cfgmgr_->firstIface();
            while (cfgIface_ = cfgmgr_->getIface()) {
                if (cfgIface_->getName() == iface_->getName())
                    break;
            }
            if (!cfgIface_) {
                ADD_FAILURE() << "Failed to find expected " << iface_->getName()
                              << " interface in CfgMgr." << endl;
                return false;
            }

            return true;
        }

        SPtr<TSrvMsg> sendAndReceive(SPtr<TSrvMsg> clntMsg) {
            EXPECT_EQ(0, transmgr_->getMsgLst().count());

            // process it through server usual routines
            transmgr_->relayMsg(clntMsg);

            EXPECT_EQ(1, transmgr_->getMsgLst().count());

            List(TSrvMsg) & msglst = transmgr_->getMsgLst();
            msglst.first();
            SPtr<TSrvMsg> rsp = msglst.get();
            if (!rsp) {
                ADD_FAILURE() << "MsgLst empty. No message response generated.";
                return 0;
            }


            if (clntMsg->getTransID() != rsp->getTransID()) {
                ADD_FAILURE() << "Returned message has transid=" << rsp->getTransID()
                              << ", but sent message with transid=" << clntMsg->getTransID();
                return 0;
            }

            return rsp;
        }

        ~ServerTest() {
            delete transmgr_;
            delete cfgmgr_;
            delete addrmgr_;
            delete ifacemgr_;
        }

        NakedSrvIfaceMgr * ifacemgr_;
        NakedSrvCfgMgr * cfgmgr_;
        NakedSrvAddrMgr * addrmgr_;
        NakedSrvTransMgr * transmgr_;
        SPtr<TIfaceIface> iface_;
        SPtr<TSrvCfgIface> cfgIface_;

        SPtr<TIPv6Addr> clntAddr_;
        SPtr<TDUID> clntDuid_;
        SPtr<TOptDUID> clntId_;
    };

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

    ASSERT_EQ(5, addrLst.count());
    addrLst.first();

    EXPECT_EQ(string("2000::400"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::401"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::404"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::405"), addrLst.get()->getPlain());
    EXPECT_EQ(string("2000::406"), addrLst.get()->getPlain());
    EXPECT_FALSE(addrLst.get()); // no additional addresses
}

TEST_F(ServerTest, CfgMgr_solicit_advertise1) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:123::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate client's message
    char empty[] = { 0x01, 0x1, 0x2, 0x3};
    SPtr<TSrvMsgSolicit> sol = new TSrvMsgSolicit(iface_->getID(), clntAddr_, empty, sizeof(empty));
    sol->addOption((Ptr*)clntId_); // include client-id

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol);
    ASSERT_TRUE(adv); // check that there is a response

    /// @todo: check that received advertise is ok
}


}
