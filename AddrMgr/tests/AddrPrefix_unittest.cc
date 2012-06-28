#include <IPv6Addr.h>
#include <AddrPrefix.h>

#include <gtest/gtest.h>

using namespace std;

namespace test {

TEST(AddrPrefixTest, constructor) {
    SPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::abcd", true);

    SPtr<TAddrPrefix> prefix = new TAddrPrefix(addr, 100, 200, 64);

    EXPECT_EQ(100u, prefix->getPref() );
    EXPECT_EQ(200u, prefix->getValid() );
    EXPECT_EQ(64u,  prefix->getLength() );
    EXPECT_EQ(string("fe80::abcd"), prefix->get()->getPlain());
}

}
