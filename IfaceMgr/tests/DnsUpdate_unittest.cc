#include <time.h>
#include "DNSUpdate.h"
#include <gtest/gtest.h>
#include "Key.h"
#include "tests/utils/poslib_utils.h"


using namespace std;
using namespace test;

namespace {

class NakedDNSUpdate : public DNSUpdate {
public:
    NakedDNSUpdate(const std::string& dns_address, const std::string& zonename,
                   const std::string& hostname, std::string hostip,
                   DnsUpdateMode updateMode, DnsUpdateProtocol proto
                   /*= DNSUPDATE_TCP*/)
        :DNSUpdate(dns_address, zonename, hostname, hostip, updateMode, proto),
         id_(0), sign_time_(0) {
    }

    virtual void sendMsg(unsigned int timeout) {
        Message_->ID = id_;
        if (sign_time_)
            Message_->tsig_rr_signtime = sign_time_;
        DNSUpdate::sendMsg(timeout);
    }

    void extractPacket() {
        DnsRR * tsig = Message_->tsig_rr;
        if (tsig) {
            if (tsig->presign_RDLENGTH) {
                if (tsig->RDATA)
                    free(tsig->RDATA);
                tsig->RDATA = (unsigned char*)memdup(tsig->presign_RDATA, tsig->presign_RDLENGTH);
                tsig->RDLENGTH = tsig->presign_RDLENGTH;
            }

        }
        buffer_ = Message_->compile(1500);
    }

    message_buff buffer_;
    uint16_t id_;
    time_t sign_time_;
};

TEST(DnsUpdateTest, AAAA) {

    DnsUpdateResult result = DNSUPDATE_SKIP;
    NakedDNSUpdate act(/*dns*/"::1", "", "foo.example.org",
                       "2001::1", DNSUPDATE_AAAA, DNSUpdate::DNSUPDATE_UDP);

    unsigned int timeout = 1000;

    message_buff expected1;
    message_buff expected2;
    // hexstring exported from wireshark

    // this also includes case where there is old AAAA record and DNS server responded
    // this version also includes delete of the old AAAA record
    hexToBin("a09328000001000000020000076578616d706c65036f7267000006000103666f6"
             "fc00c001c00fe00000000001020010000000000000000000000000001c01d001c"
             "000100001c20001020010000000000000000000000000001", expected1);

    // simple add (no previous AAAA records)
    hexToBin("a09328000001000000010000076578616d706c65036f7267000006000103666f6"
    "fc00c001c000100001c20001020010000000000000000000000000001", expected2);

    act.id_ = 0xa093; // just a random Transaction ID

    result = act.run(timeout);
    act.extractPacket();

    EXPECT_TRUE(cmpBuffers(expected1, act.buffer_) || cmpBuffers(expected2, act.buffer_));

    act.showResult(result);
}

TEST(DnsUpdateTest, TSIG_AAAA) {

    DnsUpdateResult result = DNSUPDATE_SKIP;
    NakedDNSUpdate act(/*dns*/"::1", "", "foo.example.org",
                       "2001::1", DNSUPDATE_AAAA, DNSUpdate::DNSUPDATE_UDP);

    unsigned int timeout = 1000;

    TSIGKey key("DDNS_KEY");
    key.setData("9SYMLnjK2ohb1N/56GZ5Jg==");

    act.setTSIG("DDNS_KEY", key.getPackedData(), "HMAC-MD5.SIG-ALG.REG.INT", 301 /* fudge */);

    message_buff expected1;
    message_buff expected2;
    // this also includes case where there is old AAAA record and DNS server responded
    // this version also includes delete of the old AAAA record
    hexToBin("a09428000001000000020001076578616d706c65036f7267000006000103666f6"
             "fc00c001c00fe00000000001020010000000000000000000000000001c01d001c"
             "000100001c200010200100000000000000000000000000010844444e535f4b455"
             "90000fa00ff00000000003a08484d41432d4d4435075349472d414c4703524547"
             "03494e54000000500dbd7a012d00106f9d86b83c867043dd14f047bf7014fda09"
             "400000000", expected1);

    // simple add (no previous AAAA records)
    hexToBin("a09428000001000000010001076578616d706c65036f7267000006000103666f6"
             "fc00c001c000100001c200010200100000000000000000000000000010844444e"
             "535f4b45590000fa00ff00000000003a08484d41432d4d4435075349472d414c4"
             "70352454703494e54000000500dbd7a012d0010ef1e84d5fb49f5305faa73dd7c"
             "0e1e15a09400000000", expected2);

    act.id_ = 0xa094; // just a random Transaction ID
    act.sign_time_ = 0x500dbd7a;

    result = act.run(timeout);

    // build message and store it in act.buffer_
    act.extractPacket();
    EXPECT_TRUE(cmpBuffers(expected1, act.buffer_) || cmpBuffers(expected2, act.buffer_));

    // call it second time to verify that we can build the same message
    // multiple times (test environment sanity check)
    act.extractPacket();
    EXPECT_TRUE(cmpBuffers(expected1, act.buffer_) || cmpBuffers(expected2, act.buffer_));

    act.showResult(result);
}

TEST(DnsUpdateTest, PTR) {
#if 0
    DnsUpdateModeCfg FQDNMode = static_cast<DnsUpdateModeCfg>(cfgIface->getFQDNMode());

    SPtr<TSIGKey> key = SrvCfgMgr().getKey();

    TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
    DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
    if (proto == TCfgMgr::DNSUPDATE_UDP)
        proto2 = DNSUpdate::DNSUPDATE_UDP;
    if (proto == TCfgMgr::DNSUPDATE_ANY)
        proto2 = DNSUpdate::DNSUPDATE_ANY;
    unsigned int timeout = SrvCfgMgr().getDDNSTimeout();

    // FQDNMode: 0 = NONE, 1 = PTR only, 2 = BOTH PTR and AAAA
    if ((FQDNMode == DNSUPDATE_MODE_PTR) || (FQDNMode == DNSUPDATE_MODE_BOTH)) {
        //Test for DNS update
        char zoneroot[128];
        doRevDnsZoneRoot(addr->getAddr(), zoneroot, cfgIface->getRevDNSZoneRootLength());
        /* add PTR only */
        DnsUpdateResult result = DNSUPDATE_SKIP;
        DNSUpdate *act = new DNSUpdate(dnsAddr->getPlain(), zoneroot, name, addr->getPlain(),
                                       DNSUPDATE_PTR, proto2);

        if (key) {
            act->setTSIG(key->Name_, key->getPackedData(), key->getAlgorithmText(),
                         key->Fudge_);
        }

        result = act->run(timeout);
        act->showResult(result);
        delete act;

        success = (result == DNSUPDATE_SUCCESS);
    }
#endif

}

}
