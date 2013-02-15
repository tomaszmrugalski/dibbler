#include <IPv6Addr.h>
#include <AddrClient.h>
#include <DUID.h>
#include <DHCPConst.h>
#include <SmartPtr.h>
#include <gtest/gtest.h>
#include <climits>

namespace test {
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

    EXPECT_EQ(0, client->countIA());
    EXPECT_EQ(0, client->countPD());
    EXPECT_EQ(0, client->countTA());

    EXPECT_EQ(UINT_MAX, client->getT1Timeout());
    EXPECT_EQ(UINT_MAX, client->getT2Timeout());
    EXPECT_EQ(UINT_MAX, client->getPrefTimeout());
    EXPECT_EQ(UINT_MAX, client->getValidTimeout());

    delete client;
}


} // end of anonymous namespace
