#include <IPv6Addr.h>
#include <AddrMgr.h>
#include <gtest/gtest.h>
#include <DUID.h>

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

TEST_F(AddrMgrTest, constructor) {
    // do no load the XML file
    NakedAddrMgr * mgr = new NakedAddrMgr("non-existing.xml", false);
    EXPECT_FALSE(mgr->isDone());
    EXPECT_EQ(0, mgr->countClient());
    delete mgr;

    // please load the XML file
    mgr = new NakedAddrMgr("non-existing.xml", true);
    EXPECT_FALSE(mgr->isDone());
    EXPECT_EQ(0, mgr->countClient());
    delete mgr;
}

TEST_F(AddrMgrTest, XmlLoadValidDB) {
    TAddrMgr* mgr = new NakedAddrMgr("server-AddrMgr-0.8.3.xml", true);

    ASSERT_FALSE(mgr->isDone()); // file load should be successful

/* this test validates the following XML:
  <AddrClient>
    <duid length="15">00:01:00:01:17:6c:b5:cf:f4:6d:04:fa:ce:ba:be</duid>
    <!-- 0 IA(s) -->
    <!-- 0 TA(s) -->
    <!-- 1 PD(s) -->
    <AddrPD unicast="" T1="2000" T2="3000" IAID="1" state="NOTCONFIGURED" iface="2">
      <duid length="15">00:01:00:01:17:6c:b5:cf:f4:6d:04:fa:ce:ba:be</duid>
      <AddrPrefix timestamp="1370685799" pref="8000000" valid="9000000"
                  length="96">2001:db8:3333::1fe2:0:0</AddrPrefix>
    </AddrPD>
  </AddrClient> */

    // check first client (1 PD)
    mgr->firstClient();
    SPtr<TAddrClient> client1 = mgr->getClient();
    ASSERT_TRUE(client1);

    SPtr<TDUID> exp_duid1 = new TDUID("00:01:00:01:17:6c:b5:cf:f4:6d:04:fa:ce:ba:be");
    ASSERT_TRUE(client1->getDUID());
    EXPECT_TRUE(*client1->getDUID() == *exp_duid1);
    EXPECT_EQ(0, client1->countIA());
    EXPECT_EQ(0, client1->countTA());
    EXPECT_EQ(1, client1->countPD());

    client1->firstIA();
    EXPECT_FALSE(client1->getIA());

    client1->firstTA();
    EXPECT_FALSE(client1->getTA());

    client1->firstPD();
    SPtr<TAddrIA> pd1 = client1->getPD();

    ASSERT_TRUE(pd1);
    EXPECT_EQ(2000u, pd1->getT1());
    EXPECT_EQ(3000u, pd1->getT2());
    EXPECT_EQ(1u, pd1->getIAID());
    EXPECT_EQ("", pd1->getIfacename());
    EXPECT_EQ(2, pd1->getIfindex());

    EXPECT_TRUE(*pd1->getDUID() == *exp_duid1);

    EXPECT_EQ(0, pd1->countAddr());
    EXPECT_EQ(1, pd1->countPrefix());

    pd1->firstPrefix();
    SPtr<TAddrPrefix> prefix1 = pd1->getPrefix();
    ASSERT_TRUE(prefix1);
    SPtr<TIPv6Addr> exp_addr1(new TIPv6Addr("2001:db8:3333::1fe2:0:0", true));
    EXPECT_TRUE(*prefix1->get() == *exp_addr1);
    EXPECT_EQ(96, prefix1->getLength());
    EXPECT_EQ(8000000u, prefix1->getPref());
    EXPECT_EQ(9000000u, prefix1->getValid());

    delete mgr;
}

} // end of anonymous namespace
