/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"
#include "assign_utils.h"
#include <gtest/gtest.h>

using namespace std;

namespace test {

TEST_F(ServerTest, decodeRelayForwGuessMode) {

    // check that an interface was successfully selected
    string cfg = "guess-mode\n"
                 "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:1::/64 }\n"
                 "}\n"
                 "\n"
                 "iface relay1 {"
                 "  relay REPLACE_ME\n"
                 "  interface-id 1234\n"
                 "  class { pool 2001:db8:123::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    SrvIfaceMgr().dump();

    // now generate SOLICIT
    setIface("relay1");
    clntAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr("ff05::1:3", true));
    SPtr<TSrvMsg> sol = (Ptr*)createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA

    sol->setMsgType(RELAY_FORW_MSG);

    TOptList echoOpts;
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);

    setRelayInfo(sol);

    sol->send(10000 + DHCPSERVER_PORT);

    SPtr<TSrvMsg> received = SrvIfaceMgr().select(1);

    ASSERT_TRUE(received);

    vector<TSrvMsg::RelayInfo> rcvRelay = received->RelayInfo_;

    // Check that there's info about exactly one relay
    ASSERT_EQ(rcvRelay.size(), 1u);

    // Check link-address, peer-addr and hop fields
    ASSERT_TRUE(rcvRelay[0].LinkAddr_);
    EXPECT_EQ(string(rcvRelay[0].LinkAddr_->getPlain()), "2001:db8:123::1");
    ASSERT_TRUE(rcvRelay[0].PeerAddr_);
    EXPECT_EQ(string(rcvRelay[0].PeerAddr_->getPlain()), "fe80::abcd");
    EXPECT_EQ(rcvRelay[0].Hop_, 31);

    // Check that there are no echo request options stored
    EXPECT_EQ(rcvRelay[0].EchoList_.size(), 0u);

    // Check that no remote-id was stored
    EXPECT_FALSE(received->getRemoteID());

    // Check that the packet was marked as received over proper interface
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(received->getIface());
    ASSERT_TRUE(cfgIface);
    EXPECT_EQ(cfgIface->getName(), "relay1");
}

TEST_F(ServerTest, relaySelectInterfaceIdInteger) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:0::/64 }\n"
                 "}\n"
                 "\n"
                 "iface relay1 {"
                 " relay REPLACE_ME\n"
                 " interface-id 11111\n"
                 "  class { pool 2001:db8:1::/64 }\n"
                 "}\n"
                 "iface relay2 {"
                 " relay REPLACE_ME\n"
                 " interface-id 12345\n"
                 "  class { pool 2001:db8:2::/64 }\n"
                 "}\n"
                 "iface relay3 {"
                 " relay REPLACE_ME\n"
                 " interface-id 33333\n"
                 "  class { pool 2001:db8:3::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    SrvIfaceMgr().dump();

    // now generate SOLICIT
    setIface("relay2");
    clntAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr("ff05::1:3", true));
    SPtr<TSrvMsg> sol = (Ptr*)createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA

    sol->setMsgType(RELAY_FORW_MSG);

    TOptList echoOpts;
    SPtr<TOpt> ifaceId(new TSrvOptInterfaceID(12346, NULL));
    echoOpts.push_back(ifaceId);
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);

    setRelayInfo(sol);

    // Sending packet with mismatched interface-id
    sol->send(10000 + DHCPSERVER_PORT);

    // It should not be handled
    SPtr<TSrvMsg> received = SrvIfaceMgr().select(1);
    ASSERT_FALSE(received);

    clearRelayInfo();
    sol->clearRelayInfo();
    ifaceId = SPtr<TOpt>(new TSrvOptInterfaceID(12345, NULL));
    echoOpts.clear();
    echoOpts.push_back(ifaceId);
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);
    setRelayInfo(sol);

    // Sending packet with matched interface-id
    sol->send(10000 + DHCPSERVER_PORT);

    received = SrvIfaceMgr().select(1);
    ASSERT_TRUE(received);

    vector<TSrvMsg::RelayInfo> rcvRelay = received->RelayInfo_;

    // Check that there's info about 1 relay
    ASSERT_EQ(rcvRelay.size(), 1u);

    // Check link-address, peer-addr and hop fields
    ASSERT_TRUE(rcvRelay[0].LinkAddr_);
    EXPECT_EQ(string(rcvRelay[0].LinkAddr_->getPlain()), "2001:db8:123::1");
    ASSERT_TRUE(rcvRelay[0].PeerAddr_);
    EXPECT_EQ(string(rcvRelay[0].PeerAddr_->getPlain()), "fe80::abcd");
    EXPECT_EQ(rcvRelay[0].Hop_, 31);

    // Check that there is one option stored (it should be interface-id)
    EXPECT_EQ(rcvRelay[0].EchoList_.size(), 1u);

    // Check that no remote-id was stored
    EXPECT_FALSE(received->getRemoteID());

    // Check that the packet was marked as received over proper interface
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(received->getIface());
    ASSERT_TRUE(cfgIface);
    EXPECT_EQ(cfgIface->getName(), "relay2");
}

TEST_F(ServerTest, relaySelectInterfaceIdString) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:0::/64 }\n"
                 "}\n"
                 "\n"
                 "iface relay1 {"
                 " relay REPLACE_ME\n"
                 " interface-id \"alpha\"\n"
                 "  class { pool 2001:db8:1::/64 }\n"
                 "}\n"
                 "iface relay2 {"
                 " relay REPLACE_ME\n"
                 " interface-id \"bravo-foxtrot\"\n"
                 "  class { pool 2001:db8:2::/64 }\n"
                 "}\n"
                 "iface relay3 {"
                 " relay REPLACE_ME\n"
                 " interface-id \"charlie\"\n"
                 "  class { pool 2001:db8:3::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    SrvIfaceMgr().dump();

    // now generate SOLICIT
    setIface("relay2");
    clntAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr("ff05::1:3", true));
    SPtr<TSrvMsg> sol = (Ptr*)createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA

    sol->setMsgType(RELAY_FORW_MSG);

    TOptList echoOpts;
    SPtr<TOpt> ifaceId(new TSrvOptInterfaceID("delta", 5, NULL));
    echoOpts.push_back(ifaceId);
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);

    setRelayInfo(sol);

    // Sending packet with mismatched interface-id
    sol->send(10000 + DHCPSERVER_PORT);

    // It should not be handled
    SPtr<TSrvMsg> received = SrvIfaceMgr().select(1);
    ASSERT_FALSE(received);

    clearRelayInfo();
    sol->clearRelayInfo();
    ifaceId = SPtr<TOpt>(new TSrvOptInterfaceID("bravo-foxtrot", 13, NULL));
    echoOpts.clear();
    echoOpts.push_back(ifaceId);
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);
    setRelayInfo(sol);

    // Sending packet with matched interface-id
    sol->send(10000 + DHCPSERVER_PORT);

    received = SrvIfaceMgr().select(1);
    ASSERT_TRUE(received);

    vector<TSrvMsg::RelayInfo> &rcvRelay = received->RelayInfo_;

    // Check that there's info about 1 relay
    ASSERT_EQ(rcvRelay.size(), 1u);

    // Check link-address, peer-addr and hop fields
    ASSERT_TRUE(rcvRelay[0].LinkAddr_);
    EXPECT_EQ(string(rcvRelay[0].LinkAddr_->getPlain()), "2001:db8:123::1");
    ASSERT_TRUE(rcvRelay[0].PeerAddr_);
    EXPECT_EQ(string(rcvRelay[0].PeerAddr_->getPlain()), "fe80::abcd");
    EXPECT_EQ(rcvRelay[0].Hop_, 31);

    // Check that there are no echo request options stored other than interface-id
    EXPECT_EQ(rcvRelay[0].EchoList_.size(), 1u);

    // Check that no remote-id was stored
    EXPECT_FALSE(received->getRemoteID());

    // Check that the packet was marked as received over proper interface
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(received->getIface());
    ASSERT_TRUE(cfgIface);
    EXPECT_EQ(cfgIface->getName(), "relay2");
}

TEST_F(ServerTest, relaySelectInterfaceIdHex) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:0::/64 }\n"
                 "}\n"
                 "\n"
                 "iface relay1 {"
                 " relay REPLACE_ME\n"
                 " interface-id 0x123456\n"
                 "  class { pool 2001:db8:1::/64 }\n"
                 "}\n"
                 "iface relay2 {"
                 " relay REPLACE_ME\n"
                 " interface-id 0xbc\n"
                 "  class { pool 2001:db8:2::/64 }\n"
                 "}\n"
                 "iface relay3 {"
                 " relay REPLACE_ME\n"
                 " interface-id 0x01234567890abcdef1234567890fedcba\n"
                 "  class { pool 2001:db8:3::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    SrvIfaceMgr().dump();

    // now generate SOLICIT
    setIface("relay2");
    clntAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr("ff05::1:3", true));
    SPtr<TSrvMsg> sol = (Ptr*)createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA

    sol->setMsgType(RELAY_FORW_MSG);

    TOptList echoOpts;
    char bogusIfaceId[] = { 0xff };
    echoOpts.clear();
    SPtr<TOpt> ifaceId(new TSrvOptInterfaceID(bogusIfaceId, 1, NULL));
    echoOpts.push_back(ifaceId);
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);

    setRelayInfo(sol);

    // Sending packet with mismatched interface-id
    sol->send(10000 + DHCPSERVER_PORT);

    // It should not be handled
    SPtr<TSrvMsg> received = SrvIfaceMgr().select(1);
    ASSERT_FALSE(received);

    clearRelayInfo();
    sol->clearRelayInfo();
    char validIfaceId[] = { 0xbc };
    ifaceId = SPtr<TOpt>(new TSrvOptInterfaceID(validIfaceId, 1, NULL));
    echoOpts.clear();
    echoOpts.push_back(ifaceId);
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);
    setRelayInfo(sol);

    // Sending packet with matched interface-id
    sol->send(10000 + DHCPSERVER_PORT);

    received = SrvIfaceMgr().select(1);
    ASSERT_TRUE(received);

    vector<TSrvMsg::RelayInfo> rcvRelay = received->RelayInfo_;

    // Check that there's info about 1 relay
    ASSERT_EQ(rcvRelay.size(), 1u);

    // Check link-address, peer-addr and hop fields
    ASSERT_TRUE(rcvRelay[0].LinkAddr_);
    EXPECT_EQ(string(rcvRelay[0].LinkAddr_->getPlain()), "2001:db8:123::1");
    ASSERT_TRUE(rcvRelay[0].PeerAddr_);
    EXPECT_EQ(string(rcvRelay[0].PeerAddr_->getPlain()), "fe80::abcd");
    EXPECT_EQ(rcvRelay[0].Hop_, 31);

    // Check that there are no echo request options stored other than interface-id
    EXPECT_EQ(rcvRelay[0].EchoList_.size(), 1u);

    // Check that no remote-id was stored
    EXPECT_FALSE(received->getRemoteID());

    // Check that the packet was marked as received over proper interface
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(received->getIface());
    ASSERT_TRUE(cfgIface);
    EXPECT_EQ(cfgIface->getName(), "relay2");
}


TEST_F(ServerTest, relaySelectSubnet) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:0::/64 }\n"
                 "}\n"
                 "\n"
                 "iface relay1 {"
                 "  subnet 2001:db8:1::/64\n"
                 "  relay REPLACE_ME\n"
                 "  class { pool 2001:db8:1::/64 }\n"
                 "}\n"
                 "iface relay2 {"
                 "  subnet 2001:db8:2::/64\n"
                 "  relay REPLACE_ME\n"
                 "  class { pool 2001:db8:2::/64 }\n"
                 "}\n"
                 "iface relay3 {"
                 "  subnet 2001:db8:3::/64\n"
                 "  relay REPLACE_ME\n"
                 "  class { pool 2001:db8:3::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    SrvIfaceMgr().dump();

    // now generate SOLICIT
    setIface("relay2");
    clntAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr("ff05::1:3", true));
    SPtr<TSrvMsg> sol = (Ptr*)createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA

    sol->setMsgType(RELAY_FORW_MSG);

    TOptList echoOpts;
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 31, echoOpts);

    setRelayInfo(sol);

    // Sending packet with mismatched link-address
    sol->send(10000 + DHCPSERVER_PORT);

    // It should not be handled
    SPtr<TSrvMsg> received = SrvIfaceMgr().select(1);
    ASSERT_FALSE(received);

    clearRelayInfo();
    sol->clearRelayInfo();
    addRelayInfo("2001:db8:2::1", "fe80::abcd", 31, echoOpts);
    setRelayInfo(sol);

    // Sending packet with matched interface-id
    sol->send(10000 + DHCPSERVER_PORT);

    received = SrvIfaceMgr().select(1);
    ASSERT_TRUE(received);

    vector<TSrvMsg::RelayInfo> rcvRelay = received->RelayInfo_;

    // Check that there's info about 1 relay
    ASSERT_EQ(rcvRelay.size(), 1u);

    // Check link-address, peer-addr and hop fields
    ASSERT_TRUE(rcvRelay[0].LinkAddr_);
    EXPECT_EQ(string(rcvRelay[0].LinkAddr_->getPlain()), "2001:db8:2::1");
    ASSERT_TRUE(rcvRelay[0].PeerAddr_);
    EXPECT_EQ(string(rcvRelay[0].PeerAddr_->getPlain()), "fe80::abcd");
    EXPECT_EQ(rcvRelay[0].Hop_, 31);

    // Check that there are no echo request options stored
    EXPECT_EQ(rcvRelay[0].EchoList_.size(), 0u);

    // Check that no remote-id was stored
    EXPECT_FALSE(received->getRemoteID());

    // Check that the packet was marked as received over proper interface
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(received->getIface());
    ASSERT_TRUE(cfgIface);
    EXPECT_EQ(cfgIface->getName(), "relay2");
}


}
