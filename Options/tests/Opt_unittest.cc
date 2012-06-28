#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "Opt.h"
#include "OptAddr.h"
#include "OptGeneric.h"

using namespace std;

namespace {

TEST(OptTest, basic) {
    SPtr<TIPv6Addr> addr = new TIPv6Addr("2001:db8:1::dead:beef", true);
    TOptAddr* opt = new TOptAddr(OPTION_UNICAST, addr, NULL);

    EXPECT_EQ(OPTION_UNICAST, opt->getOptType());

    delete opt;
}

TEST(OptTest, suboptions) {
    char buf[128];
    for (int i=0; i<128; i++)
        buf[i] = i;

    TOptGeneric* opt1 = new TOptGeneric(1, buf, 8, NULL);
    SPtr<TOpt> opt2 = new TOptGeneric(2, buf+8, 4, NULL);
    SPtr<TOpt> opt3 = new TOptGeneric(3, buf+12, 2, NULL);
    SPtr<TOpt> opt4 = new TOptGeneric(4, buf+14, 1, NULL);
    SPtr<TOpt> opt5 = new TOptGeneric(5, buf+15, 0, NULL);

    EXPECT_EQ(0, opt1->countOption());

    opt1->addOption(opt2);

    EXPECT_EQ(1, opt1->countOption());

    delete opt1;
}


}
