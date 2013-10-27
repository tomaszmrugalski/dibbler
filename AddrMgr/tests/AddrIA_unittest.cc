#include <IPv6Addr.h>
#include <AddrIA.h>
#include <DUID.h>

#include <gtest/gtest.h>

using namespace std;

namespace test {

TEST(AddrIATest, constructor) {
    SPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::abcd", true);

    const char duidData[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6};
    SPtr<TDUID> duid = new TDUID(duidData, sizeof(duidData));

    SPtr<TAddrIA> ia = new TAddrIA("eth0", 1, IATYPE_IA, addr, duid, 100, 200, 300);

    EXPECT_EQ(1, ia->getIfindex());
    EXPECT_EQ(100u, ia->getT1());
    EXPECT_EQ(200u, ia->getT2());
    EXPECT_EQ(300u, ia->getIAID());
    EXPECT_EQ(string("fe80::abcd"), ia->getSrvAddr()->getPlain());
    EXPECT_EQ(0, ia->countAddr());
}

}
