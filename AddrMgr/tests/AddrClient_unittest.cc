#include <IPv6Addr.h>
#include <AddrClient.h>
#include <DUID.h>
#include <SmartPtr.h>
#include <gtest/gtest.h>

namespace {
    class AddrClientTest : public ::testing::Test {
    public:
        AddrClientTest() { }
    };

TEST_F(AddrClientTest, constructor) {

    const char duidData[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6};
    SPtr<TDUID> duid = new TDUID(duidData, sizeof(duidData));
    TAddrClient *client = new TAddrClient(duid);

    SPtr<TDUID> d = client->getDUID();
    EXPECT_TRUE(d);

    EXPECT_EQ(duid->getLen(), d->getLen());
    EXPECT_TRUE(duid == d);

    EXPECT_EQ(client->countIA(), 0);
    EXPECT_EQ(client->countPD(), 0);
    EXPECT_EQ(client->countTA(), 0);

    EXPECT_EQ(client->getT1Timeout(), DHCPV6_INIFINITY);
    EXPECT_EQ(client->getT2Timeout(), DHCPV6_INIFINITY);
    EXPECT_EQ(client->getPrefTimeout(), DHCPV6_INIFINITY);
    EXPECT_EQ(client->getValidTimeout(), DHCPV6_INIFINITY);

    delete client;
}


} // end of anonymous namespace
