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

TEST_F(ServerTest, SARR_prefix_single_class) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  pd-class {\n"
        "    pd-pool 2001:db8:123::/48\n"
        "    pd-length 64\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)pd_); // include IA_PD

    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_PD> rcvPD = (Ptr*) adv->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123::", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123:ffff:ffff:ffff:ffff:ffff", true);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 101, 102, SERVER_DEFAULT_MAX_PREF, SERVER_DEFAULT_MAX_VALID, 64) );

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)pd_);

    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvPD = (Ptr*) reply->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    // server should return T1 = 101, becasue SERVER_DEFAULT_MIN_T1(5) < 101 < SERVER_DEFAULT_MAX_T1 (3600)
    // server should return T2 = 101, becasue SERVER_DEFAULT_MIN_T1(10) < 102 < SERVER_DEFAULT_MAX_T1 (5400)
    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 101, 102, SERVER_DEFAULT_MAX_PREF, SERVER_DEFAULT_MAX_VALID, 64) );
}

TEST_F(ServerTest, SARR_prefix_single_class_params) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  t1 1000\n"
        "  t2 2000\n"
        "  preferred-lifetime 3000\n"
        "  valid-lifetime 4000\n"
        "  pd-class {\n"
        "    pd-pool 2001:db8:123::/48\n"
        "    pd-length 65\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)pd_); // include IA_NA
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_PD> rcvPD = (Ptr*) adv->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123::", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123:ffff:ffff:ffff:ffff:ffff", true);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 65));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)pd_);
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvPD = (Ptr*) reply->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 65));
}

TEST_F(ServerTest, SARR_prefix_inpool_reservation) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  t1 1000\n"
        "  t2 2000\n"
        "  preferred-lifetime 3000\n"
        "  valid-lifetime 4000\n"
        "  pd-class {\n"
        "    pd-pool 2001:db8:123::/48\n"
        "    pd-length 68\n"
        "  }\n"
        "  client duid 00:01:00:0a:0b:0c:0d:0e:0f {\n"
        "    prefix 2001:db8:123:babe::/67\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)pd_); // include IA_NA
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_PD> rcvPD = (Ptr*) adv->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123:babe::", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123:babe::", true);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 67));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)pd_);
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvPD = (Ptr*) reply->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 67));
}

TEST_F(ServerTest, SARR_prefix_inpool_reservation_negative) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  t1 1000\n"
        "  t2 2000\n"
        "  preferred-lifetime 3000\n"
        "  valid-lifetime 4000\n"
        "  pd-class {\n"
        "    pd-pool 2001:db8:123::/48\n"
        "    pd-length 56\n"
        "  }\n"
        "  client duid 00:01:00:00:00:00:00:00:00 {\n" // not our DUID
        "    prefix 2002:babe::/68\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)pd_); // include PD_NA
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);
    const uint8_t prefixLen = 68;
    SPtr<TIPv6Addr> prefix = new TIPv6Addr("2002:babe::", true);
    SPtr<TSrvOptIAPrefix> optPrefix = new TSrvOptIAPrefix(prefix, prefixLen, 1000, 2000, &(*sol));
    pd_->addOption((Ptr*)optPrefix);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_PD> rcvPD = (Ptr*) adv->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    SPtr<TSrvOptIAPrefix> rcvOptPrefix = (Ptr*)rcvPD->getOption(OPTION_IAPREFIX);
    ASSERT_TRUE(rcvOptPrefix);
    cout << "Requested " << prefix->getPlain() << "/" << prefixLen
         << ", received " << rcvOptPrefix->getPrefix()->getPlain() << "/"
         << (int)rcvOptPrefix->getPrefixLength() << endl;

    if (prefix->getPlain() == rcvOptPrefix->getPrefix()->getPlain()) {
        ADD_FAILURE() << "Assigned address that was reserved for someone else.";
    }

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2001:db8:123::", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2001:db8:123:ffff:ffff:ffff:ffff:ffff", true);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 56));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)pd_);
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvPD = (Ptr*) reply->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 56));
}

TEST_F(ServerTest, SARR_prefix_outpool_reservation) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
        "  t1 1000\n"
        "  t2 2000\n"
        "  preferred-lifetime 3000\n"
        "  valid-lifetime 4000\n"
        "  pd-class {\n"
        "    pd-pool 2001:db8:123::/48\n"
        "    pd-length 50\n"
        "  }\n"
        "  client duid 00:01:00:0a:0b:0c:0d:0e:0f {\n"
        "    prefix 2002:babe::/32\n"
        "  }\n"
        "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    // now generate SOLICIT
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)pd_); // include PD_NA
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // check that there is an ADVERTISE response

    SPtr<TSrvOptIA_PD> rcvPD = (Ptr*) adv->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    SPtr<TIPv6Addr> minRange = new TIPv6Addr("2002:babe::", true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr("2002:babe::", true);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 32));

    cout << "REQUEST" << endl;

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption((Ptr*)pd_);
    pd_->setIAID(100);
    pd_->setT1(101);
    pd_->setT2(102);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvPD = (Ptr*) reply->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, 100, 1000, 2000, 3000, 4000, 32));
}

}
