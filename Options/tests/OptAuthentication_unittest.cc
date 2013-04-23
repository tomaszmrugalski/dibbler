#include <gtest/gtest.h>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "OptAddr.h"
#include "OptAuthentication.h"

namespace test {

TEST(OptAuthTest, constructor) {

    char expData[] = { 0, 11, // option type
                       0, 11, // option length
                       3, // protocol
                       0, // algorithm
                       1, // RDM = monotonic,
                       1, 2, 3, 4, 5, 6, 7, 8 }; // replay detection data
                       
                       

    char buf[100];
    memset(buf, 0, sizeof(buf));
    SPtr<TOptAuthentication> opt = new TOptAuthentication(AUTH_PROTO_RECONFIGURE_KEY,
                                                          0, AUTH_REPLAY_MONOTONIC, NULL);
    opt->setReplayDetection(0x0102030405060708);
    char* ptr = opt->storeSelf(buf);
    EXPECT_EQ(15u, opt->getSize());
    EXPECT_EQ(ptr, buf + 15);
    EXPECT_TRUE(0 == memcmp(buf, expData, 15));
}

}
