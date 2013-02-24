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

TEST_F(ServerTest, decodeRelayForw) {

    // check that an interface was successfully selected
    string cfg = "iface REPLACE_ME {\n"
                 "  class { pool 2001:db8:1::/64 }\n"
                 "}\n"
                 "\n"
                 "iface relay1 {"
                 " relay REPLACE_ME\n"
                 " interface-id 1234\n"
                 "  class { pool 2001:db8:123::/64 }\n"
                 "}\n";

    ASSERT_TRUE( createMgrs(cfg) );

    SrvIfaceMgr().dump();

    // now generate SOLICIT
    ASSERT_TRUE(SrvIfaceMgr().getIfaceByName("relay1"));
    iface_ = SrvIfaceMgr().getIfaceByName("relay1");
    clntAddr_ = SPtr<TIPv6Addr>(new TIPv6Addr("ff05::1:3", true));
    SPtr<TSrvMsg> sol = (Ptr*)createSolicit();
    sol->addOption((Ptr*)clntId_); // include client-id
    sol->addOption((Ptr*)ia_); // include IA_NA

    sol->setMsgType(RELAY_FORW_MSG);

    List(TOptGeneric) echoOpts;
    addRelayInfo("2001:db8:123::1", "fe80::abcd", 0, 31, echoOpts);

    setRelayInfo(sol);

    sol->send(547);

    SPtr<TSrvMsg> received = SrvIfaceMgr().select(1);

    if (received) {
        cout << "#### Received" << endl;
    } else {
        cout << "#### NOT Received" << endl;
    }



}

}
