#include <time.h>
#include "DNSUpdate.h"
#include <gtest/gtest.h>
#include "Key.h"
#include "ClntIfaceMgr/ClntIfaceMgr.h"

using namespace std;

namespace {

// This unit-test verifies that the number of bits needed to
// differentiate specified number of prefixes is calculated
// properly
TEST(ClntIfaceMgr, bits) {
    string xmlFile = "ClntIfaceMgr.xml";
    EXPECT_NO_THROW(TClntIfaceMgr::instanceCreate(xmlFile));

    EXPECT_EQ(0, TClntIfaceMgr::instance().numBits(0));
    EXPECT_EQ(1, TClntIfaceMgr::instance().numBits(1));
    EXPECT_EQ(1, TClntIfaceMgr::instance().numBits(2));
    EXPECT_EQ(2, TClntIfaceMgr::instance().numBits(4));
    EXPECT_EQ(3, TClntIfaceMgr::instance().numBits(5));
    EXPECT_EQ(3, TClntIfaceMgr::instance().numBits(8));
    EXPECT_EQ(4, TClntIfaceMgr::instance().numBits(9));
    EXPECT_EQ(4, TClntIfaceMgr::instance().numBits(16));
    EXPECT_EQ(5, TClntIfaceMgr::instance().numBits(17));
    EXPECT_EQ(5, TClntIfaceMgr::instance().numBits(32));
    EXPECT_EQ(6, TClntIfaceMgr::instance().numBits(33));
    EXPECT_EQ(6, TClntIfaceMgr::instance().numBits(64));
    EXPECT_EQ(7, TClntIfaceMgr::instance().numBits(65));
    EXPECT_EQ(7, TClntIfaceMgr::instance().numBits(128));
}

// This test verifies that the client code splits delegated prefix
// correctly into several subprefixes. This test verifies the simpler
// case when the delegated prefix ends on full byte boundary (/48, /56, /64 etc).
//
// This test checks the following case:
//
// We have delegated prefix of 2001:db8:1234::/48 that's going to
// be split into 3 subprefixes:
// 2001:db8:1234:100::
// 2001:db8:1234:200::
// 2001:db8:1234:300::
TEST(ClntIfaceMgr, calculateSubprefixFullByte) {
    string xmlFile = "ClntIfaceMgr.xml";
    EXPECT_NO_THROW(TClntIfaceMgr::instanceCreate(xmlFile));

    int subprefixLen = 0;
    SPtr<TIPv6Addr> delegated(new TIPv6Addr("2001:db8:1234::", true));
    SPtr<TIPv6Addr> subprefix;

    EXPECT_NO_THROW(subprefix = TClntIfaceMgr::instance()
                    .calculateSubprefix(delegated, 48, 3, 1, subprefixLen));

    EXPECT_EQ("2001:db8:1234:100::", string(subprefix->getPlain()));
    EXPECT_EQ(64, subprefixLen);

    EXPECT_NO_THROW(subprefix = TClntIfaceMgr::instance()
                    .calculateSubprefix(delegated, 48, 3, 2, subprefixLen));

    EXPECT_EQ("2001:db8:1234:200::", string(subprefix->getPlain()));
    EXPECT_EQ(64, subprefixLen);

    EXPECT_NO_THROW(subprefix = TClntIfaceMgr::instance()
                    .calculateSubprefix(delegated, 48, 3, 3, subprefixLen));

    EXPECT_EQ("2001:db8:1234:300::", string(subprefix->getPlain()));
    EXPECT_EQ(64, subprefixLen);
}

// This test verifies that the client code splits delegated prefix
// correctly into several subprefixes. This test verifies the more complex
// case when the delegated prefix does not end on byte boundary (e.g. /60) and
// the interface-id has to be encoded on several bits of one byte and
// remaining bits of another
//
// We have delegated prefix of 2001:db8:1234:c000:/50 that's going to
// be split into 3 subprefixes:
// 2001:db8:1234:c040::
// 2001:db8:1234:c080::
// 2001:db8:1234:c0c0::
TEST(ClntIfaceMgr, calculateSubprefixMidByte) {
    string xmlFile = "ClntIfaceMgr.xml";
    EXPECT_NO_THROW(TClntIfaceMgr::instanceCreate(xmlFile));

    int subprefixLen = 0;
    SPtr<TIPv6Addr> delegated(new TIPv6Addr("2001:db8:1234:c000::", true));
    SPtr<TIPv6Addr> subprefix;

    EXPECT_NO_THROW(subprefix = TClntIfaceMgr::instance()
                    .calculateSubprefix(delegated, 50, 3, 1, subprefixLen));
    EXPECT_EQ("2001:db8:1234:c040::", string(subprefix->getPlain()));
    EXPECT_EQ(64, subprefixLen);

    EXPECT_NO_THROW(subprefix = TClntIfaceMgr::instance()
                    .calculateSubprefix(delegated, 50, 3, 2, subprefixLen));
    EXPECT_EQ("2001:db8:1234:c080::", string(subprefix->getPlain()));
    EXPECT_EQ(64, subprefixLen);

    EXPECT_NO_THROW(subprefix = TClntIfaceMgr::instance()
                    .calculateSubprefix(delegated, 50, 3, 3, subprefixLen));
    EXPECT_EQ("2001:db8:1234:c0c0::", string(subprefix->getPlain()));
    EXPECT_EQ(64, subprefixLen);
}


}
