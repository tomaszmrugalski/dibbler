#include <iostream>
#include <gtest/gtest.h>
#include "assign_utils.h"

#include "OptAddrLst.h"
#include "OptStatusCode.h"

using namespace std;

namespace test {

bool ServerTest::checkIA_NA(SPtr<TSrvOptIA_NA> ia, SPtr<TIPv6Addr> minRange,
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
        ADD_FAILURE() << "Response with transid=" << std::hex << clntMsg->getTransID() << " not found.";
        return 0;
    }

    if (clntMsg->getTransID() != rsp->getTransID()) {
        ADD_FAILURE() << "Returned message has transid=" << rsp->getTransID()
                      << ", but sent message with transid=" << clntMsg->getTransID();
        return 0;
    }

    return rsp;
}

bool ServerTest::createMgrs(std::string config) {

    if (!iface_) {
        ADD_FAILURE() << "No suitable interface detected: all are down or not running";
        return false;
    }

    // try to repalace IFACE name with an actual string name
    size_t pos = config.find("REPLACE_ME");
    if (pos != std::string::npos) {
        config.replace(pos, 10, iface_->getName());
    }

    std::ofstream cfgfile("testdata/server.conf");
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
                      << " interface in CfgMgr." << std::endl;
        return false;
    }

    return true;
}


}
