#include "RelTransMgr.h"
#include "RelIfaceMgr.h"
#include "RelCfgMgr.h"
#include "RelMsgGeneric.h"
#include "hex.h"

#include <gtest/gtest.h>

using namespace std;

namespace {

    class NakedRelIfaceMgr: public TRelIfaceMgr {
    public:
        NakedRelIfaceMgr(const std::string& xmlFile)
            : TRelIfaceMgr(xmlFile) {
            TRelIfaceMgr::Instance = this;
        }
        ~NakedRelIfaceMgr() {
            TRelIfaceMgr::Instance = NULL;
        }
    };

    class NakedRelCfgMgr : public TRelCfgMgr {
    public:
        NakedRelCfgMgr(const std::string& config, const std::string& dbfile)
            :TRelCfgMgr(config, dbfile) { }
    };

    class NakedRelTransMgr : public TRelTransMgr {
    public:
        NakedRelTransMgr(const std::string& xml)
            :TRelTransMgr(xml) { }

        using TRelTransMgr::getLinkAddrFromDuid;
        using TRelTransMgr::getLinkAddrFromSrcAddr;
        using TRelTransMgr::getClientLinkLayerAddr;
    };


TEST(RelTransMgrTest, getLinkAddrFromDUID) {

    TRelCfgMgr::instanceCreate("dummy.conf", "dummy.xml");

    uint8_t data_llt[] = {
        1,  // type 1 = SOLICIT
        0xca, 0xfe, 0x01, // trans-id = 0xcafe01
        0, 1, // option type 1 (client-id)
        0, 14, // option lenth 14
        0, 1, // DUID type (1 = DUID-LLT)
        0, 1, // hardware type
        0x15, 0x9e, 0x3c, 0x8f, // timestamp
        1, 2, 3, 4, 5, 6, // MAC address
        0, 3, // option type 3 (IA_NA)
        0, 12, // option length 12
        0, 0, 0, 1, // iaid = 1
        0, 0, 0, 0, // T1 = 0
        0, 0, 0, 0  // T2 = 0
    };

    uint8_t data_ll[] = {
        1,  // type 1 = SOLICIT
        0xca, 0xfe, 0x01, // trans-id = 0xcafe01
        0, 1, // option type 1 (client-id)
        0, 10, // option lenth 10
        0, 3, // DUID type (3 = DUID-LL)
        0, 1, // hardware type
        1, 2, 3, 4, 5, 6, // MAC address
        0, 3, // option type 3 (IA_NA)
        0, 12, // option length 12
        0, 0, 0, 1, // iaid = 1
        0, 0, 0, 0, // T1 = 0
        0, 0, 0, 0  // T2 = 0
    };

    uint8_t data_en[] = {
        1,  // type 1 = SOLICIT
        0xca, 0xfe, 0x01, // trans-id = 0xcafe01
        0, 1, // option type 1 (client-id)
        0, 14, // option lenth 10
        0, 2, // DUID type (2 = DUID-EN)
        0, 1, // hardware type
        0x1, 0x02, 0x03, 0x04, // EN=0x1020304
        5, 6, 7, 8, 9, 10, // MAC address
        0, 3, // option type 3 (IA_NA)
        0, 12, // option length 12
        0, 0, 0, 1, // iaid = 1
        0, 0, 0, 0, // T1 = 0
        0, 0, 0, 0  // T2 = 0
    };

    uint8_t expected_clientaddr[] =
    {
        0, OPTION_CLIENT_LINKLAYER_ADDR, // option code
        0, 8, // option lenth
        0, 1, // hardware type
        1, 2, 3, 4, 5, 6 // MAC address (6 letters)
    };

    SPtr<TIPv6Addr> peer(new TIPv6Addr("fe80::1122:33ff:fe44:5566"));

    SPtr<TRelMsg> msg1(new TRelMsgGeneric(1, peer, (char*)data_llt, sizeof(data_llt)));
    SPtr<TRelMsg> msg2(new TRelMsgGeneric(1, peer, (char*)data_ll, sizeof(data_ll)));
    SPtr<TRelMsg> msg3(new TRelMsgGeneric(1, peer, (char*)data_en, sizeof(data_en)));

    NakedRelTransMgr transmgr("./tmp.xml");

    SPtr<TOpt> opt;

    // Case 1: DUID-LLT
    // message1 must have client-id option, and it should be DUID-LLT
    opt = msg1->getOption(OPTION_CLIENTID);
    ASSERT_TRUE(opt);

    ASSERT_NO_THROW(opt = transmgr.getLinkAddrFromDuid(opt));
    ASSERT_TRUE(opt);
    EXPECT_EQ(12, opt->getSize());

    char output[100];
    char* ptr;
    memset(output, 0, 100);
    ASSERT_NO_THROW(ptr = opt->storeSelf(output));
    EXPECT_EQ(12, ptr - output);

    std::cout << "Received: " << hexToText((uint8_t*)output, 12, true, false) << std::endl;
    std::cout << "Expected: "<< hexToText((uint8_t*)expected_clientaddr, 12, true, false) << std::endl;
    EXPECT_FALSE(memcmp(output, expected_clientaddr, 12));

    // Case 2: DUID-LL
    opt = msg2->getOption(OPTION_CLIENTID);
    ASSERT_TRUE(opt);

    ASSERT_NO_THROW(opt = transmgr.getLinkAddrFromDuid(opt));
    ASSERT_TRUE(opt);
    EXPECT_EQ(12, opt->getSize());

    memset(output, 0, 100);
    ASSERT_NO_THROW(ptr = opt->storeSelf(output));
    EXPECT_EQ(12, ptr - output);

    std::cout << "Received: " << hexToText((uint8_t*)output, 12, true, false) << std::endl;
    std::cout << "Expected: "<< hexToText((uint8_t*)expected_clientaddr, 12, true, false) << std::endl;
    EXPECT_FALSE(memcmp(output, expected_clientaddr, 12));

    // Case 3: DUID-EN, should not return anything
    opt = msg3->getOption(OPTION_CLIENTID);
    ASSERT_TRUE(opt);

    ASSERT_NO_THROW(opt = transmgr.getLinkAddrFromDuid(opt));
    EXPECT_FALSE(opt);
}

}
