#include "IPv6Addr.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"
#include "OptDUID.h"
#include "OptAddrLst.h"
#include "SrvOptTA.h"
#include "DHCPConst.h"
#include "HostRange.h"
#include "assign_utils.h"
#include <gtest/gtest.h>

using namespace std;

namespace test {



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
