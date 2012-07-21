#include "DNSUpdate.h"

#include <gtest/gtest.h>

using namespace std;

namespace test {

TEST(DnsUpdateTest, AAAA) {

        DnsUpdateResult result = DNSUPDATE_SKIP;
        DNSUpdate *act = new DNSUpdate(/*dns*/"::1", "", "foo.example.org",
                                       "2001::1",
                                       DNSUPDATE_AAAA, DNSUpdate::DNSUPDATE_UDP);

        unsigned int timeout = 1000;

#if 0
	if (key) {
	    act->setTSIG(key->Name_, key->getPackedData(), key->getAlgorithmText(),
			 key->Fudge_);
	}
#endif

        result = act->run(timeout);
        act->showResult(result);
        delete act;
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
