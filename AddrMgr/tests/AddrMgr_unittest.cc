#include <IPv6Addr.h>
#include <AddrMgr.h>
#include <gtest/gtest.h>

namespace test {

    class AddrMgrTest : public ::testing::Test {
    public:
        AddrMgrTest() { }
    };

    class NakedAddrMgr : public TAddrMgr {
    public:
        NakedAddrMgr(const std::string& addrdb, bool loadfile):
            TAddrMgr(addrdb, loadfile) { 
            
        }
        virtual void print(std::ostream& s) {
            
        }

    };

TEST_F(AddrMgrTest, constructor1) {
    NakedAddrMgr * mgr = new NakedAddrMgr("testfile.xml", false);

    delete mgr;
}

TEST_F(AddrMgrTest, xmlLoad1) {


}

} // end of anonymous namespace
