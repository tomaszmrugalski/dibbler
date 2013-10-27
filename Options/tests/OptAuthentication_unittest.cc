#include <gtest/gtest.h>
#include <vector>

#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "OptAddr.h"
#include "OptAuthentication.h"

namespace test {

TEST(OptAuthTest, constructorNone) {

    char expData[] = { 0, 11, // option type
                       0, 11, // option length
                       0, // protocol
                       0, // algorithm
                       1, // RDM = monotonic,
                       1, 2, 3, 4, 5, 6, 7, 8 }; // replay detection data

    char buf[100];
    memset(buf, 0, sizeof(buf));
    SPtr<TOptAuthentication> opt = new TOptAuthentication(AUTH_PROTO_NONE,
                                                          0, AUTH_REPLAY_MONOTONIC, NULL);
    opt->setReplayDetection(0x0102030405060708);
    char* ptr = opt->storeSelf(buf);
    EXPECT_EQ(15u, opt->getSize());
    EXPECT_EQ(ptr, buf + 15);
    EXPECT_TRUE(0 == memcmp(buf, expData, 15));
}

TEST(OptAuthTest, constructorReconfigureKey) {

    char expData1[] = { 0, 11, // option type
                        0, TOptAuthentication::OPT_AUTH_FIXED_SIZE + RECONFIGURE_KEY_AUTHINFO_SIZE, // option length
                        3, // protocol
                        0, // algorithm
                        1, // RDM = monotonic,
                        1, 2, 3, 4, 5, 6, 7, 8, // replay detection data
                        1, // type 1 = reconfigure-key
                        0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 9, 8, // 16byte key
                        7, 6, 5, 4, 3, 2, 1, 0 };

    char expData2[] = { 0, 11, // option type
                        0, TOptAuthentication::OPT_AUTH_FIXED_SIZE + RECONFIGURE_KEY_AUTHINFO_SIZE, // option length
                        3, // protocol
                        0, // algorithm
                        1, // RDM = monotonic,
                        1, 2, 3, 4, 5, 6, 7, 8, // replay detection data
                        2, // type 2 = HMAC-MD5 digest
                        0, 0, 0, 0, 0, 0, 0, 0, // zeroed - HMAC-MD5 digest will be calculated
                        0, 0, 0, 0, 0, 0, 0, 0 }; // later

    std::vector<uint8_t> digest(17,0);
    for (int i=0; i<16; i++) {
        digest[i+1] = 15 - i;
    }

    char buf[100];
    memset(buf, 0, sizeof(buf));

    digest[0] = 1; // try with key type 1 = reconfigure-key
    SPtr<TOptAuthentication> opt = new TOptAuthentication(AUTH_PROTO_RECONFIGURE_KEY,
                                                          0, AUTH_REPLAY_MONOTONIC, NULL);
    opt->setReplayDetection(0x0102030405060708);
    opt->setPayload(digest);
    char* ptr = opt->storeSelf(buf);
    size_t expLen = TOpt::OPTION6_HDR_LEN + TOptAuthentication::OPT_AUTH_FIXED_SIZE
        + RECONFIGURE_KEY_AUTHINFO_SIZE;
    EXPECT_EQ(expLen, opt->getSize());
    EXPECT_EQ(ptr, buf + expLen);
    EXPECT_TRUE(0 == memcmp(buf, expData1, expLen));

    digest[0] = 2; // try with key type 2 = HMAC-MD5 digest
    opt = new TOptAuthentication(AUTH_PROTO_RECONFIGURE_KEY,
                                 0, AUTH_REPLAY_MONOTONIC, NULL);
    opt->setReplayDetection(0x0102030405060708);
    opt->setPayload(digest);
    ptr = opt->storeSelf(buf);
    expLen = TOpt::OPTION6_HDR_LEN + TOptAuthentication::OPT_AUTH_FIXED_SIZE
        + RECONFIGURE_KEY_AUTHINFO_SIZE;
    EXPECT_EQ(expLen, opt->getSize());
    EXPECT_EQ(ptr, buf + expLen);
    EXPECT_TRUE(0 == memcmp(buf, expData2, expLen));
}

TEST(OptAuthTest, constructorDelayedAuth) {

    char expData1[] = { 0, 11, // option type
                        0, TOptAuthentication::OPT_AUTH_FIXED_SIZE, // option length
                        2, // protocol
                        1, // algorithm (HMAC-MD5)
                        1, // RDM = monotonic,
                        0, 0, 0, 0, 0, 0, 0, 0 // replay detection data
    };

    const std::string realm = "DHCP realm";

    char expData2[] = { 0, 11, // option type
                        0, static_cast<uint8_t>(TOptAuthentication::OPT_AUTH_FIXED_SIZE +
                                                realm.size() +
                                                DELAYED_AUTH_DIGEST_SIZE +
                                                DELAYED_AUTH_KEY_ID_SIZE), // option length
                        2, // protocol
                        1, // algorithm (HMAC-MD5)
                        1, // RDM = monotonic,
                        1, 2, 3, 4, 5, 6, 7, 8, // replay detection data
                        'D', 'H', 'C', 'P', ' ', 'r', 'e', 'a', 'l', 'm',
                        0, 0, 0, 0, // key ID 0x0
                        0, 0, 0, 0, 0, 0, 0, 0, // zeroed - HMAC-MD5 digest will be calculated
                        0, 0, 0, 0, 0, 0, 0, 0 }; // later

    char buf[100];
    memset(buf, 0, sizeof(buf));

    // Try empty option (sent in SOLICIT)
    SPtr<TOptAuthentication> opt = new TOptAuthentication(AUTH_PROTO_DELAYED,
                                                          1, AUTH_REPLAY_MONOTONIC, NULL);
    opt->setReplayDetection(0x0);
    opt->setRealm(std::string(""));
    char* ptr = opt->storeSelf(buf);
    size_t expLen = TOpt::OPTION6_HDR_LEN + TOptAuthentication::OPT_AUTH_FIXED_SIZE;
    EXPECT_EQ(expLen, opt->getSize());
    EXPECT_EQ(ptr, buf + expLen);
    EXPECT_TRUE(0 == memcmp(buf, expData1, expLen));

    opt = new TOptAuthentication(AUTH_PROTO_DELAYED,
                                 1, AUTH_REPLAY_MONOTONIC, NULL);
    opt->setReplayDetection(0x0102030405060708);
    opt->setRealm(realm);
    ptr = opt->storeSelf(buf);
    expLen = TOpt::OPTION6_HDR_LEN + TOptAuthentication::OPT_AUTH_FIXED_SIZE
        + realm.size() + + DELAYED_AUTH_DIGEST_SIZE + DELAYED_AUTH_KEY_ID_SIZE;
    EXPECT_EQ(expLen, opt->getSize());
    EXPECT_EQ(ptr - buf, expLen);
    EXPECT_TRUE(0 == memcmp(buf, expData2, expLen));
}

TEST(OptAuthTest, constructorDibbler) {

    char expData[] = { 0, 11, // option type
                       0, 15, // option length
                       4, // protocol
                       0, // algorithm
                       1, // RDM = monotonic,
                       1, 2, 3, 4, 5, 6, 7, 8, // replay detection data
                       0, 0, 0, 0}; // SPI = 0 (because there's no parent message)

    char buf[100];
    memset(buf, 0, sizeof(buf));
    SPtr<TOptAuthentication> opt = new TOptAuthentication(AUTH_PROTO_DIBBLER,
                                                          0, AUTH_REPLAY_MONOTONIC, NULL);
    opt->setReplayDetection(0x0102030405060708);
    char* ptr = opt->storeSelf(buf);
    EXPECT_EQ(19u, opt->getSize());
    EXPECT_EQ(ptr, buf + 19);
    EXPECT_TRUE(0 == memcmp(buf, expData, 19));
}

}
