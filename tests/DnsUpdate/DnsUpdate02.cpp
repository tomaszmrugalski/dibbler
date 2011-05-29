#include <iostream>
#include "DNSUpdate.h"
#include "Logger.h"

using namespace std;
int main(int argc, const char *argv[]) {

    string dnsAddr   = "2000::1";
    string zonename  = "example.org.";
    string hostname  = "troi.example.org.";
    string hostip    = "2000::dead:beef";

    DNSUpdate *act = new DNSUpdate(dnsAddr, zonename, hostname, hostip, 
				   DNSUPDATE_AAAA, DNSUpdate::DNSUPDATE_TCP);
    int result = act->run();
    delete act;
    Log(Debug) << "RESULT: PTR=" << result << LogEnd;

    DNSUpdate *act2= new DNSUpdate(dnsAddr, zonename, hostname, hostip,
				   DNSUPDATE_AAAA, DNSUpdate::DNSUPDATE_UDP);
    int result2 = act2->run();
    delete act2;
    Log(Debug) << "RESULT: PTR=" << result2 << LogEnd;

}
