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

}
