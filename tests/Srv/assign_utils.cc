#include <iostream>
#include <gtest/gtest.h>
#include "assign_utils.h"

#include "OptAddrLst.h"
#include "OptStatusCode.h"
#include <unistd.h>
#include <string>

using namespace std;

namespace test {

Pkt6Info::Pkt6Info(int iface, char* msg, int size, SPtr<TIPv6Addr> addr, int port)
    :Iface_(iface), Data_(size), Addr_(addr), Port_(port) {
    memcpy(&Data_[0], msg, size);
}

bool NakedSrvIfaceMgr::send(int iface, char *msg, int size, SPtr<TIPv6Addr> addr, int port) {

    Pkt6Info x(iface, msg, size, addr, port);

    sent_pkts_.push_back(x);

    return TSrvIfaceMgr::send(iface, msg, size, addr, port);
}

int NakedSrvIfaceMgr::receive(unsigned long timeout, char* buf, int& bufsize,
                              SPtr<TIPv6Addr> peer, SPtr<TIPv6Addr> myaddr) {

    return TSrvIfaceMgr::receive(timeout, buf, bufsize, peer, myaddr);
}

ServerTest::ServerTest() {
    clntDuid_ = new TDUID("00:01:00:0a:0b:0c:0d:0e:0f");
    clntId_ = new TOptDUID(OPTION_CLIENTID, clntDuid_, NULL);
    clntAddr_ = new TIPv6Addr("fe80::1234", true);

    ifacemgr_ = new NakedSrvIfaceMgr("testdata/server-IfaceMgr.xml");

    // try to pick up an up and running interface
    ifacemgr_->firstIface();
    while ( (iface_ = ifacemgr_->getIface()) && (!iface_->flagUp() || !iface_->flagRunning())) {
    }
}

void ServerTest::createIAs(TMsg* msg) {
    ia_iaid_ = 123;
    ia_ = new TSrvOptIA_NA(ia_iaid_, 100, 200, msg);
    ta_iaid_ = 456;
    ta_ = new TOptTA(ta_iaid_, msg);
    pd_iaid_ = 789;
    pd_ = new TSrvOptIA_PD(pd_iaid_, 100, 200, msg);
}

SPtr<TSrvMsgSolicit> ServerTest::createSolicit() {
    char empty[] = { SOLICIT_MSG, 0x1, 0x2, 0x3};
    SPtr<TSrvMsgSolicit> sol =
        new TSrvMsgSolicit(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*sol));
    return sol;
}

SPtr<TSrvMsgRequest> ServerTest::createRequest() {
    char empty[] = { REQUEST_MSG, 0x1, 0x2, 0x4};
    SPtr<TSrvMsgRequest> request =
        new TSrvMsgRequest(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*request));
    return request;
}

SPtr<TSrvMsgRenew> ServerTest::createRenew() {
    char empty[] = { RENEW_MSG, 0x1, 0x2, 0x5};
    SPtr<TSrvMsgRenew> renew =
        new TSrvMsgRenew(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*renew));
    return renew;
}

SPtr<TSrvMsgRebind> ServerTest::createRebind() {
    char empty[] = { REBIND_MSG, 0x1, 0x2, 0x6};
    SPtr<TSrvMsgRebind> rebind =
        new TSrvMsgRebind(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*rebind));
    return rebind;
}

SPtr<TSrvMsgRelease> ServerTest::createRelease() {
    char empty[] = { RELEASE_MSG, 0x1, 0x2, 0x7};
    SPtr<TSrvMsgRelease> release =
        new TSrvMsgRelease(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*release));
    return release;
}

SPtr<TSrvMsgDecline> ServerTest::createDecline() {
    char empty[] = { DECLINE_MSG, 0x1, 0x2, 0x8};
    SPtr<TSrvMsgDecline> decline =
        new TSrvMsgDecline(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*decline));
    return decline;
}

SPtr<TSrvMsgConfirm> ServerTest::createConfirm() {
    char empty[] = { CONFIRM_MSG, 0x1, 0x2, 0x9};
    SPtr<TSrvMsgConfirm> confirm =
        new TSrvMsgConfirm(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*confirm));
    return confirm;
}

SPtr<TSrvMsgInfRequest> ServerTest::createInfRequest() {
    char empty[] = { INFORMATION_REQUEST_MSG, 0x1, 0x2, 0xa};
    SPtr<TSrvMsgInfRequest> infrequest =
        new TSrvMsgInfRequest(iface_->getID(), clntAddr_, empty, sizeof(empty));
    createIAs(&(*infrequest));
    return infrequest;
}

bool ServerTest::checkIA_NA(SPtr<TSrvOptIA_NA> ia, SPtr<TIPv6Addr> minRange,
                            SPtr<TIPv6Addr> maxRange, uint32_t iaid, uint32_t t1,
                            uint32_t t2, uint32_t pref, uint32_t valid) {
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


bool ServerTest::checkIA_PD(SPtr<TSrvOptIA_PD> pd, SPtr<TIPv6Addr> minRange,
                            SPtr<TIPv6Addr> maxRange, uint32_t iaid, uint32_t t1, uint32_t t2,
                            uint32_t pref, uint32_t valid, uint8_t prefixLen) {
    THostRange range(minRange, maxRange);
    int count = 0;

    EXPECT_EQ(iaid, pd->getIAID());
    EXPECT_EQ(t1, pd->getT1());
    EXPECT_EQ(t2, pd->getT2());

    pd->firstOption();
    while (SPtr<TOpt> option = pd->getOption()) {
        switch (option->getOptType()) {
        case OPTION_STATUS_CODE: {
            SPtr<TOptStatusCode> optCode = (Ptr*)option;
            EXPECT_EQ(STATUSCODE_SUCCESS, optCode->getCode());
            break;
        }
        case OPTION_IAPREFIX: {
            SPtr<TSrvOptIAPrefix> optPrefix = (Ptr*)option;
            cout << "Checking received prefix " << optPrefix->getPrefix()->getPlain()
                 << "/" << (int)optPrefix->getPrefixLength() << endl;
            EXPECT_TRUE( range.in(optPrefix->getPrefix()) );
            EXPECT_EQ(pref, optPrefix->getPref() );
            EXPECT_EQ(valid, optPrefix->getValid() );
            EXPECT_EQ(prefixLen, optPrefix->getPrefixLength());
            count++;
            break;
        }
        default:
            ADD_FAILURE() << "Unexpected option type " << option->getOptType()
                          << " received in IA_PD(iaid=" << ia_->getIAID() << ")";
            break;
        }
    }
    return (count>0);
}


SPtr<TSrvMsg> ServerTest::sendAndReceive(SPtr<TSrvMsg> clntMsg,
                                         unsigned int expectedMsgCount/* = 1*/) {
    EXPECT_EQ(expectedMsgCount - 1, transmgr_->getMsgLst().size());

    // process it through server usual routines
    transmgr_->relayMsg(clntMsg);

    EXPECT_EQ(expectedMsgCount, transmgr_->getMsgLst().size());

    SrvMsgList& msglst = transmgr_->getMsgLst();

    SPtr<TSrvMsg> rsp;
    for (SrvMsgList::const_iterator it = msglst.begin(); it != msglst.end(); ++it) {
        if ((*it)->getTransID() == clntMsg->getTransID()) {
            rsp = *it;
        }
    }

    if (!rsp) {
        ADD_FAILURE() << "Response with transid=" << std::hex << clntMsg->getTransID()
                      << " not found.";
        return SPtr<TSrvMsg>(); // NULL
    }

    if (clntMsg->getTransID() != rsp->getTransID()) {
        ADD_FAILURE() << "Returned message has transid=" << rsp->getTransID()
                      << ", but sent message with transid=" << clntMsg->getTransID();
        return SPtr<TSrvMsg>(); // NULL
    }

    return rsp;
}

void NakedSrvTransMgr::sendPacket(SPtr<TSrvMsg> msg) {
    std::cout << "Pretending to send packet" << std::endl;
    MsgLst_.push_back(msg);
}


bool ServerTest::createMgrs(std::string config) {

    if (!iface_) {
        ADD_FAILURE() << "No suitable interface detected: all are down or not running";
        return false;
    }

    // try to repalace IFACE name with an actual string name
    size_t pos;
    do {
        pos = config.find("REPLACE_ME");
        if (pos != std::string::npos) {
            config.replace(pos, 10, iface_->getName());
        }
    } while (pos != std::string::npos);

    std::ofstream cfgfile("testdata/server.conf");
    cfgfile << config;
    cfgfile.close();

    unlink("server-cache.xml");

    cfgmgr_ = new NakedSrvCfgMgr("testdata/server.conf", "testdata/server-CfgMgr.xml");
    addrmgr_ = new NakedSrvAddrMgr("testdata/server-AddrMgr.xml", false); // don't load db
    transmgr_ = new NakedSrvTransMgr("testdata/server-TransMgr.xml", 10000 + DHCPSERVER_PORT);

    if (cfgmgr_->isDone()) {
        ADD_FAILURE() << "CfgMgr reported problems and is shutting down.";
        return false;
    }

    cfgmgr_->firstIface();
    while (cfgIface_ = cfgmgr_->getIface()) {
        if (cfgIface_->getName() == iface_->getName())
            break;
    }
    if (!cfgIface_) {
        ADD_FAILURE() << "Failed to find expected " << iface_->getName()
                      << " interface in CfgMgr." << std::endl;
        return false;
    }

    return true;
}

void ServerTest::addRelayInfo(const std::string& linkAddr, const std::string& peerAddr,
                              uint8_t hopCount, const TOptList& echoList) {
    TSrvMsg::RelayInfo x;
    x.LinkAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr(linkAddr.c_str(), true));
    x.PeerAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr(peerAddr.c_str(), true));
    x.Len_ = 0;
    x.Hop_ = hopCount;
    x.EchoList_ = echoList;

    relayInfo_.push_back(x);
}

void ServerTest::clearRelayInfo() {
    relayInfo_.clear();
}

void ServerTest::setRelayInfo(SPtr<TSrvMsg> msg) {
    for (std::vector<TSrvMsg::RelayInfo>::const_iterator relay = relayInfo_.begin();
         relay != relayInfo_.end(); ++relay) {
        msg->addRelayInfo(relay->LinkAddr_, relay->PeerAddr_, relay->Hop_,
                          relay->EchoList_);
    }
}

void ServerTest::setIface(const std::string& name) {
    ASSERT_TRUE(SrvCfgMgr().getIfaceByName(name));
    ASSERT_NE(-1, SrvCfgMgr().getIfaceByName("relay1")->getRelayID());
    iface_ = (Ptr*) SrvIfaceMgr().getIfaceByID(SrvCfgMgr().getIfaceByName(name)->getRelayID());
}

void ServerTest::sendHex(const std::string& src_addr, uint16_t src_port,
                         const std::string& dst_addr, uint16_t dst_port,
                         const std::string& iface_name,
                         const std::string& hex_data) {

    // convert hex data to binary data first
    if (hex_data.length()%2) {
        ADD_FAILURE() << "Specified hex string (" << hex_data << " has length " <<
            hex_data.length() << ", even length required.";
        return;
    }
    size_t len = hex_data.length()/2;
    char* buffer = new char[len];
    TDUID tmp(hex_data.c_str());
    EXPECT_EQ(tmp.storeSelf(static_cast<char*>(buffer)), buffer + len);

    SPtr<TIfaceIface> iface = SrvIfaceMgr().getIfaceByName(iface_name);
    ASSERT_TRUE(iface);

    SPtr<TIPv6Addr> addr = new TIPv6Addr(dst_addr.c_str(), true);
    bool status = SrvIfaceMgr().send(iface->getID(), buffer, len, addr, dst_port);

    EXPECT_TRUE(status);

    delete [] buffer;
}

TOptPtr ServerTest::createPrefix(const std::string& addr_txt, uint8_t len, uint32_t pref, uint32_t valid) {
    SPtr<TIPv6Addr> addr(new TIPv6Addr(addr_txt.c_str(), true));
    TOptPtr iaprefix = new TSrvOptIAPrefix(addr, len, pref, valid, NULL);
    return (iaprefix);
}


void ServerTest::prefixText(const std::string& config,
                            const TOptPtr& pd_to_be_sent,
                            const std::string& min_range,
                            const std::string& max_range,
                            uint8_t expected_prefix_len,
                            uint32_t expected_iaid,
                            uint32_t expected_t1,
                            uint32_t expected_t2,
                            uint32_t expected_pref,
                            uint32_t expected_valid) {

    // Create configuration with the following config file
    ASSERT_TRUE( createMgrs(config) );

    // Get the server configuration. We'll use it later for verification
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(iface_->getID());
    ASSERT_TRUE(cfgIface);
    cfgIface->firstPD();
    SPtr<TSrvCfgPD> cfgPD = cfgIface->getPD();
    ASSERT_TRUE(cfgPD);

    // Now generate SOLICIT with a single IA_PD and one IAPREFIX hint in it
    // That's a perfect hint (valid, within scope, exact length, not used)
    SPtr<TSrvMsgSolicit> sol = createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption(pd_to_be_sent); // include IA_PD

    SPtr<TSrvMsgAdvertise> adv = (Ptr*)sendAndReceive((Ptr*)sol, 1);
    ASSERT_TRUE(adv); // Check that there is a response

    SPtr<TSrvOptIA_PD> rcvPD = (Ptr*) adv->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);

    // The server should return exactly the hint, because it is available
    SPtr<TIPv6Addr> minRange = new TIPv6Addr(min_range.c_str(), true);
    SPtr<TIPv6Addr> maxRange = new TIPv6Addr(max_range.c_str(), true);

    // Check that the IA_PD included in the response matches expectations
    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, expected_iaid, expected_t1,
                            expected_t2, expected_pref, expected_valid,
                            expected_prefix_len));

    // Nothing should be assigned (this is SOLICIT/ADVERTISE only)
    EXPECT_EQ(0u, cfgPD->getAssignedCount());

    // now generate REQUEST
    SPtr<TSrvMsgRequest> req = createRequest();
    req->addOption((Ptr*)clntId_);
    req->addOption(pd_to_be_sent);

    ASSERT_TRUE(adv->getOption(OPTION_SERVERID));
    req->addOption(adv->getOption(OPTION_SERVERID));

    // ... and get REPLY from the server
    cout << "Pretending to send REQUEST" << endl;
    SPtr<TSrvMsgReply> reply = (Ptr*)sendAndReceive((Ptr*)req, 2);
    ASSERT_TRUE(reply);

    rcvPD = (Ptr*) reply->getOption(OPTION_IA_PD);
    ASSERT_TRUE(rcvPD);


    EXPECT_TRUE( checkIA_PD(rcvPD, minRange, maxRange, expected_iaid, expected_t1,
                            expected_t2, expected_pref, expected_valid,
                            expected_prefix_len));

    // Check that the lease was indeed assigned
    EXPECT_EQ(1u, cfgPD->getAssignedCount());

    // let's release it
    SPtr<TSrvMsgRelease> rel = createRelease();
    rel->addOption((Ptr*)clntId_);
    rel->addOption(req->getOption(OPTION_SERVERID));
    rcvPD->delOption(OPTION_STATUS_CODE);
    rel->addOption((Ptr*)rcvPD);

    cout << "Pretending to send RELEASE" << endl;
    SPtr<TSrvMsgReply> releaseReply = (Ptr*)sendAndReceive((Ptr*)rel, 3);

    // Check that the lease is now released.
    EXPECT_EQ(0u, cfgPD->getAssignedCount());
}

ServerTest::~ServerTest() {
    delete transmgr_;
    delete cfgmgr_;
    delete addrmgr_;
    delete ifacemgr_;
}

}
