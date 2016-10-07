#include <time.h>
#include "DNSUpdate.h"
#include <gtest/gtest.h>
#include "Key.h"
#include "ClntIfaceMgr/ClntIfaceMgr.h"

using namespace std;

namespace {

TEST(ClntIfaceMgr, bits) {
    string xmlFile = "ClntIfaceMgr.xml";

    EXPECT_NO_THROW(TClntIfaceMgr::instanceCreate(xmlFile));
}

}
