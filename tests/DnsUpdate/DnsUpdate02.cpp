#include <iostream>
#include "DNSUpdate.h"
#include "Logger.h"

using namespace std;

bool DnsUpdate_test(DNSUpdate::DnsUpdateProtocol proto, bool dhcid, bool tsig) {

    string dnsAddr   = "2000::1";
    string zonename  = "example.org.";
    string hostname  = "troi.example.org.";
    string hostip    = "2000::dead:beef";

    char duid[]="this is my duid";
    int duidLen = strlen(duid);

    DNSUpdate *act = new DNSUpdate(dnsAddr, zonename, hostname, hostip, 
				   DNSUPDATE_AAAA, proto);
    if (dhcid)
        act->addDHCID(duid, duidLen);

    int result = act->run(5);
    delete act;
    Log(Debug) << "RESULT: PTR=" << result << LogEnd;

    return true;
}

int main(int argc, const char *argv[]) {

    DnsUpdate_test(DNSUpdate::DNSUPDATE_UDP, false, false);
    DnsUpdate_test(DNSUpdate::DNSUPDATE_TCP, false, false);

    DnsUpdate_test(DNSUpdate::DNSUPDATE_UDP, true, false);
    DnsUpdate_test(DNSUpdate::DNSUPDATE_UDP, false, true);
}
