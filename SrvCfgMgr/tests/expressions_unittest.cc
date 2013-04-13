#include <gtest/gtest.h>

#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "NodeClientSpecific.h"
#include "SrvMsg.h"
#include "SrvMsgSolicit.h"
#include "OptGeneric.h"
#include "OptUserClass.h"
#include "OptVendorClass.h"
#include "OptVendorSpecInfo.h"
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

    // This is content for tested options
    char payload1[] = { 0x41, 0x42, 0x43, 0x44 }; // ABCD
    char payload2[] = { 0, 0, 0x11, 0x8b,   // uint32_t = 4491 (enterprise-id)
			0, 3,               // uint16_t = 3 (data length)
			0x45, 0x46, 0x47 }; // "EFG"
    char payload3[] = { 0x48, 0x49, 0x4a, 0x4b, 0x4c }; // HIJKL

    SPtr<TIPv6Addr> addr = new TIPv6Addr("2001:db8:1::1", true);
    SPtr<TSrvMsg> msg = new TSrvMsgSolicit(1, addr, buffer, 0);

    const uint32_t enterprise = 1701;

    SPtr<TOpt> opt;

    // add user-class option
    opt = new TOptUserClass(OPTION_USER_CLASS, payload1, sizeof(payload1), NULL);
    msg->addOption(opt);

    // add vendor-class option
    opt = new TOptVendorClass(OPTION_VENDOR_CLASS, payload2, sizeof(payload2), NULL);
    msg->addOption(opt);

    // add vendor-spec option
    opt = new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, enterprise, 1,
				 payload3, sizeof(payload3), NULL);
    msg->addOption(opt);

    // check that the enterprise-num can be extracted properly
    NodeClientSpecific c(NodeClientSpecific::CLIENT_VENDOR_CLASS_ENTERPRISE_NUM);
    EXPECT_EQ("4491", c.exec(msg));

    // check that the content of the vendor spec info option can be extracted properly
    NodeClientSpecific d(NodeClientSpecific::CLIENT_VENDOR_CLASS_DATA);
    EXPECT_EQ("EFG", d.exec(msg));


    // check that the enterprise-num can be extracted properly
    NodeClientSpecific e(NodeClientSpecific::CLIENT_VENDOR_SPEC_ENTERPRISE_NUM);
    EXPECT_EQ("1701", e.exec(msg));

    // check that the content of the vendor spec info option can be extracted properly
    NodeClientSpecific f(NodeClientSpecific::CLIENT_VENDOR_SPEC_DATA);
    EXPECT_EQ("HIJKL", f.exec(msg));

}

}
