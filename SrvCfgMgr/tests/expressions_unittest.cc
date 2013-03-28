#include <gtest/gtest.h>

#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "NodeClientSpecific.h"
#include "SrvMsg.h"
#include "SrvMsgSolicit.h"
#include "OptGeneric.h"
#include <fstream>

using namespace std;

namespace {

class ExpressionsTest : public ::testing::Test {
public:
    ExpressionsTest() {
	TSrvIfaceMgr::instanceCreate(SRVIFACEMGR_FILE);
	if ( SrvIfaceMgr().isDone() ) {
	    return;
	}

	string config = " iface eth0 { class { pool 2001:db8:1::/64 } }";
	string cfgfile = "testdata/server-2.conf";

	ofstream file(cfgfile.c_str());
	file << config;
	file.close();


	TSrvCfgMgr::instanceCreate(cfgfile, SRVCFGMGR_FILE);
	if ( SrvCfgMgr().isDone() ) {
	    return;
	}
    }

};

TEST_F(ExpressionsTest, simple) {

    char buffer[] = { 0 };

    char payload[] = { 0x45, 0x46, 0x47, 0x48 }; // ABCD

    SPtr<TIPv6Addr> addr = new TIPv6Addr("2001:db8:1::1", true);
    SPtr<TSrvMsg> msg = new TSrvMsgSolicit(1, addr, buffer, 0);

    const uint32_t enterprise = 1701;

    SPtr<TOpt> vendorOpt = new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, enterprise, 0,
						  payload, sizeof(payload), NULL);
    msg->addOption(vendorOpt);

    NodeClientSpecific::analyseMessage(msg);

    // check that the enterprise-num can be extracted properly
    NodeClientSpecific x(NodeClientSpecific::CLIENT_VENDOR_CLASS_ENTERPRISE_NUM);
    EXPECT_EQ("1701", x.exec(msg));

}

}
