#include <iostream>
#include "DNSUpdate.h"

using namespace std;
int main() {

    char * dnsAddr   = "2000::1";
    char * zonename  = "0.0.00.0.0.ip6.arpa";
    char * hostname  = "fqdn";
    char * hostip    = "2000::123";
    int numOfRecords = 1;

    DNSUpdate *act = new DNSUpdate(dnsAddr, zonename, hostname, hostip, "2h", numOfRecords);
    int result = act->run();
    delete act;

}
