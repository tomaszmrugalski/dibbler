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

int NakedSrvIfaceMgr::receive(unsigned long timeout, char* buf, int& bufsize, SPtr<TIPv6Addr> peer) {
    return TSrvIfaceMgr::receive(timeout, buf, bufsize, peer);
}

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

ServerTest::~ServerTest() {
    delete transmgr_;
    delete cfgmgr_;
    delete addrmgr_;
    delete ifacemgr_;
}

}
