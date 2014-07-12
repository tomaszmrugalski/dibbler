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
#include "SrvOptInterfaceID.h"
#include "assign_utils.h"
#include <gtest/gtest.h>

using namespace std;

namespace test {

TEST_F(ServerTest, parseDoubleRelay) {


    // check that an interface was successfully selected
    string cfg =
        "iface REPLACE_ME { class { pool 2001:db8:dead::/64 } } "
        "iface \"relay1\" { "
        "relay \"REPLACE_ME\" "
        "interface-id \"ISAM144|299|ipv6|nt:vp:1:110\" "
        "}"
        "iface \"relay2\" {"
        "relay relay1 "

        "interface-id \"ISAM144 eth 1/1/05/01\" "

        "class { "
        "    pool 2001:888:db8:1::/64"
        "}"
        "}";

#if 0
 "guess-mode\n"
                 "iface REPLACE_ME {\n"
                 "  unicast 2001:db8:100:f101::3\n"
                 "  class { pool 2001:db8:1::/64 }\n"
                 "}\n"
                 "\n"
                 "iface relay1 {"
                 "  relay REPLACE_ME\n"
                 "  interface-id 1234\n"
                 "  class { pool 2001:db8:123::/64 }\n"
                 "}\n";
#endif

    ASSERT_TRUE( createMgrs(cfg) );

    SrvIfaceMgr().dump();

    // Send doubly encapsulated relay, with
    sendHex("2001:888:db8:1::5", 547, "ff05::1:3", 10000 + DHCPSERVER_PORT, iface_->getName(),
            "0c01200108880db800010000000000000000fe80000000000000020021fffe5c18a900"
            "09007d0c0000000000000000000000000000000000fe80000000000000020021fffe5c"
            "18a9001200154953414d3134342065746820312f312f30352f30310025000400000de9"
            "00090036016b4fe20001000e0001000118b033410000215c18a90003000c00000001ff"
            "ffffffffffffff00080002000000060006001700f200f30012001c4953414d3134347c"
            "3239397c697076367c6e743a76703a313a313130002500120000197f0001000118b033"
            "410000215c18a9");

    SPtr<TSrvMsg> received = SrvIfaceMgr().select(1);

    ASSERT_TRUE(received);

    vector<TSrvMsg::RelayInfo> &rcvRelay = received->RelayInfo_;

    // Check that there's info about exactly one relay
    ASSERT_EQ(rcvRelay.size(), 2u);

    // Check link-address, peer-addr and hop fields
    ASSERT_TRUE(rcvRelay[0].LinkAddr_);
    EXPECT_EQ(string(rcvRelay[0].LinkAddr_->getPlain()), "2001:888:db8:1::");
    ASSERT_TRUE(rcvRelay[0].PeerAddr_);
    EXPECT_EQ(string(rcvRelay[0].PeerAddr_->getPlain()), "fe80::200:21ff:fe5c:18a9");
    EXPECT_EQ(rcvRelay[0].Hop_, 1);

    // Check that there are 2 options: interface-id and remote-id
    EXPECT_EQ(rcvRelay[0].EchoList_.size(), 2u);

    // Check that the packet was marked as received over proper interface
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(received->getIface());
    ASSERT_TRUE(cfgIface);
    EXPECT_EQ(cfgIface->getName(), "relay2");

    // PHASE 1: Parse outer relay

    // Check that interface-id was stored
    SPtr<TSrvOptInterfaceID> interfaceid = (Ptr*) TOpt::getOption(rcvRelay[0].EchoList_,
                                                                  OPTION_INTERFACE_ID);
    ASSERT_TRUE(interfaceid);
    const char* ifaceid = "ISAM144|299|ipv6|nt:vp:1:110";
    TSrvOptInterfaceID expected_ifaceId(ifaceid, strlen(ifaceid), 0);
    EXPECT_EQ(*interfaceid, expected_ifaceId);

    // Check that remote-id was stored
    EXPECT_TRUE(received->getRemoteID());

    interfaceid = (Ptr*) TOpt::getOption(rcvRelay[1].EchoList_,
                                         OPTION_INTERFACE_ID);
    ASSERT_TRUE(interfaceid);
    ifaceid = "ISAM144 eth 1/1/05/01";
    TSrvOptInterfaceID expected_ifaceId2(ifaceid, strlen(ifaceid), 0);
    EXPECT_EQ(*interfaceid, expected_ifaceId2);

    // Check that remote-id was stored
    EXPECT_TRUE(received->getRemoteID());
}

}
