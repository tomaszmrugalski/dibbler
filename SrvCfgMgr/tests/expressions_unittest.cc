#include <gtest/gtest.h>

#include "NodeClientSpecific.h"
#include "SrvMsg.h"
#include "OptGeneric.h"

using namespace std;

namespace {

TEST(ExpressionsTest, simple) {

    char payload[] = { 0x45, 0x46, 0x47, 0x48 }; // ABCD

    SPtr<TIPv6Addr> addr = new TIPv6Addr("2001:db8:1::1", true);
    SPtr<TSrvMgr> msg = (Ptr*) new TSrvMsgAdvertise(1, addr);

    const uint32_t enterprise = 1701;

    SPtr<TOpt> vendorOpt = new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, enterprise,
						  payload, sizeof(payload), NULL);
    msg->addOption(vendorOpt);

    NodeClientSpecific::analyseMessage(msg);

    // check that the enterprise-num can be extracted properly
    NodeClientSpecific x(CLIENT_VENDOR_CLASS_ENTERPRISE_NUM);
    EXPECT_EQ("1701", x.exec(msg));

}

}
