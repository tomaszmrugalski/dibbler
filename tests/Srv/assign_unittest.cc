#include "IPv6Addr.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"
#include "OptDUID.h"
#include "OptAddrLst.h"
#include "OptStatusCode.h"
#include "SrvMsgSolicit.h"
#include "SrvMsgAdvertise.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRelease.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgDecline.h"
#include "SrvMsgInfRequest.h"
#include "SrvOptIA_NA.h"
#include "SrvOptIA_PD.h"
#include "SrvOptTA.h"
#include "DHCPConst.h"
#include "HostRange.h"

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
            clntDuid_ = new TDUID("00:01:00:0a:0b:0c:0d:0e:0f");
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

            unlink("server-cache.xml");

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

        void createIAs(TMsg * msg) {
            ia_iaid_ = 123;
            ia_ = new TSrvOptIA_NA(ia_iaid_, 100, 200, msg);
            ta_iaid_ = 456;
            ta_ = new TOptTA(ta_iaid_, msg);
            pd_iaid_ = 789;
            pd_ = new TSrvOptIA_PD(pd_iaid_, 100, 200, msg);
        }

        SPtr<TSrvMsgSolicit> createSolicit() {

            char empty[] = { SOLICIT_MSG, 0x1, 0x2, 0x3};
            SPtr<TSrvMsgSolicit> sol = new TSrvMsgSolicit(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*sol));
            return sol;
        }

        SPtr<TSrvMsgRequest> createRequest() {
            char empty[] = { REQUEST_MSG, 0x1, 0x2, 0x4};
            SPtr<TSrvMsgRequest> request = new TSrvMsgRequest(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*request));
            return request;
        }

        SPtr<TSrvMsgRenew> createRenew() {
            char empty[] = { RENEW_MSG, 0x1, 0x2, 0x5};
            SPtr<TSrvMsgRenew> renew = new TSrvMsgRenew(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*renew));
            return renew;
        }

        SPtr<TSrvMsgRebind> createRebind() {
            char empty[] = { REBIND_MSG, 0x1, 0x2, 0x6};
            SPtr<TSrvMsgRebind> rebind = new TSrvMsgRebind(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*rebind));
            return rebind;
        }

        SPtr<TSrvMsgRelease> createRelease() {
            char empty[] = { RELEASE_MSG, 0x1, 0x2, 0x7};
            SPtr<TSrvMsgRelease> release = new TSrvMsgRelease(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*release));
            return release;
        }

        SPtr<TSrvMsgDecline> createDecline() {
            char empty[] = { DECLINE_MSG, 0x1, 0x2, 0x8};
            SPtr<TSrvMsgDecline> decline = new TSrvMsgDecline(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*decline));
            return decline;
        }

        SPtr<TSrvMsgConfirm> createConfirm() {
            char empty[] = { CONFIRM_MSG, 0x1, 0x2, 0x9};
            SPtr<TSrvMsgConfirm> confirm = new TSrvMsgConfirm(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*confirm));
            return confirm;
        }

        SPtr<TSrvMsgInfRequest> createInfRequest() {
            char empty[] = { INFORMATION_REQUEST_MSG, 0x1, 0x2, 0xa};
            SPtr<TSrvMsgInfRequest> infrequest = new TSrvMsgInfRequest(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*infrequest));
            return infrequest;
        }

        SPtr<TSrvMsg> sendAndReceive(SPtr<TSrvMsg> clntMsg, int expectedMsgCount = 1) {
            EXPECT_EQ(expectedMsgCount - 1, transmgr_->getMsgLst().count());

            // process it through server usual routines
            transmgr_->relayMsg(clntMsg);

            EXPECT_EQ(expectedMsgCount, transmgr_->getMsgLst().count());

            List(TSrvMsg) & msglst = transmgr_->getMsgLst();
            msglst.first();
            SPtr<TSrvMsg> rsp;
            while (rsp = msglst.get()) {
                if (rsp->getTransID() == clntMsg->getTransID())
                    break;
            }

            if (!rsp) {
                ADD_FAILURE() << "Response with transid=" << hex << clntMsg->getTransID() << " not found.";
                return 0;
            }

            if (clntMsg->getTransID() != rsp->getTransID()) {
                ADD_FAILURE() << "Returned message has transid=" << rsp->getTransID()
                              << ", but sent message with transid=" << clntMsg->getTransID();
                return 0;
            }

            return rsp;
        }

        bool checkIA_NA(SPtr<TSrvOptIA_NA> ia, SPtr<TIPv6Addr> minRange,
                        SPtr<TIPv6Addr> maxRange, uint32_t iaid, uint32_t t1, uint32_t t2,
                        uint32_t pref, uint32_t valid) {
            THostRange range(minRange, maxRange);
            int count = 0;

            EXPECT_EQ(iaid, ia->getIAID());
            EXPECT_EQ(t1, ia->getT1());
            EXPECT_EQ(t2, ia->getT2());

            ia->firstOption();
            while (SPtr<TOpt> option = ia->getOption()) {
                switch (option->getOptType()) {
                case OPTION_STATUS_CODE: {
                    SPtr<TOptStatusCode> optCode = (Ptr*)option;
                    EXPECT_EQ(STATUSCODE_SUCCESS, optCode->getCode());
                    break;
                }
                case OPTION_IAADDR: {
                    SPtr<TSrvOptIAAddress> optAddr = (Ptr*)option;
                    cout << "Checking received address " << optAddr->getAddr()->getPlain() << endl;
                    EXPECT_TRUE( range.in(optAddr->getAddr()) );
                    EXPECT_EQ(pref, optAddr->getPref() );
                    EXPECT_EQ(valid, optAddr->getValid() );
                    count++;
                    break;
                }
                default:
                    ADD_FAILURE() << "Unexpected option type " << option->getOptType()
                                  << " received in IA_NA(iaid=" << ia_->getIAID();
                    break;
                }
            }
            return (count>0);
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

        SPtr<TSrvOptIA_NA> ia_;
        SPtr<TSrvOptIA_PD> pd_;
        SPtr<TOptTA>    ta_;
        uint32_t ia_iaid_;
        uint32_t ta_iaid_;
        uint32_t pd_iaid_;
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

TEST_F(ServerTest, SARR_single_class) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:123::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA

    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_NA> rcvIA = (Ptr*) adv->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123::", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123::ffff:ffff:ffff:ffff", true);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 101, 102, SERVER_DEFAULT_MAX_PREF, SERVER_DEFAULT_MAX_VALID) );

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)ia_);

    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvIA = (Ptr*) reply->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    // server should return T1 = 101, becasue SERVER_DEFAULT_MIN_T1(5) < 101 < SERVER_DEFAULT_MAX_T1 (3600)
    // server should return T2 = 101, becasue SERVER_DEFAULT_MIN_T1(10) < 102 < SERVER_DEFAULT_MAX_T1 (5400)
    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 101, 102, SERVER_DEFAULT_MAX_PREF, SERVER_DEFAULT_MAX_VALID) );
}

TEST_F(ServerTest, SARR_single_class_params) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  t1 1000\n"
                 "  t2 2000\n"
                 "  preferred-lifetime 3000\n"
                 "  valid-lifetime 4000\n"
                 "  class { pool 2001:db8:123::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_NA> rcvIA = (Ptr*) adv->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123::", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123::ffff:ffff:ffff:ffff", true);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)ia_);
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvIA = (Ptr*) reply->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));
}

TEST_F(ServerTest, SARR_inpool_reservation) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  t1 1000\n"
        "  t2 2000\n"
        "  preferred-lifetime 3000\n"
        "  valid-lifetime 4000\n"
        "  class { pool 2001:db8:123::/64 }\n"
        "  client duid 00:01:00:0a:0b:0c:0d:0e:0f {\n"
        "    address 2001:db8:123::babe\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_NA> rcvIA = (Ptr*) adv->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123::babe", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123::babe", true);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)ia_);
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvIA = (Ptr*) reply->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));
}

TEST_F(ServerTest, SARR_inpool_reservation_negative) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  t1 1000\n"
        "  t2 2000\n"
        "  preferred-lifetime 3000\n"
        "  valid-lifetime 4000\n"
        "  class { pool 2001:db8:123::/64 }\n"
        "  client duid 00:01:00:00:00:00:00:00:00 {\n" // not our DUID
        "    address 2001:db8:123::babe\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);
    SPtr<TIPv6Addr> addr = new TIPv6Addr("2001:db8:123::babe", true);
    SPtr<TSrvOptIAAddress> optAddr = new TSrvOptIAAddress(addr, 1000, 2000, &(*sol));
    ia_->addOption((Ptr*)optAddr);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_NA> rcvIA = (Ptr*) adv->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    SPtr<TSrvOptIAAddress> rcvOptAddr = (Ptr*)rcvIA->getOption(OPTION_IAADDR);
    ASSERT_TRUE(rcvOptAddr);
    cout << "Requested " << addr->getPlain() << ", received " << rcvOptAddr->getAddr()->getPlain() << endl;

    if (addr->getPlain() == rcvOptAddr->getAddr()->getPlain()) {
        ADD_FAILURE() << "Assigned address that was reserved for someone else.";
    }

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123::babe", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123::babe", true);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)ia_);
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvIA = (Ptr*) reply->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));
}

TEST_F(ServerTest, SARR_outpool_reservation) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  t1 1000\n"
        "  t2 2000\n"
        "  preferred-lifetime 3000\n"
        "  valid-lifetime 4000\n"
        "  class { pool 2001:db8:123::/64 }\n"
        "  client duid 00:01:00:0a:0b:0c:0d:0e:0f {\n"
        "    address 2002::babe\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_NA> rcvIA = (Ptr*) adv->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2002::babe", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2002::babe", true);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)ia_);
    ia_->setIAID(100);
    ia_->setT1(101);
    ia_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvIA = (Ptr*) reply->getOption(OPTION_IA_NA);
    ASSERT_TRUE(rcvIA);

    EXPECT_TRUE( checkIA_NA(rcvIA, minRange, maxRange, 100, 1000, 2000, 3000, 4000));
}


}
