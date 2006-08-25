#include <iostream>
#include "DNSUpdate.h"
#include "Logger.h"

using namespace std;
int main() {

    char * dnsAddr   = "2000::1";
    char * zonename  =  "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.2.ip6.arpa.";
    char * hostname  = "troi.example.com.";
    char * hostip    = "2000::333";
    int numOfRecords = 1;

    Log(Debug) << "TEST" << LogEnd;

    DNSUpdate *act = new DNSUpdate(dnsAddr, zonename, hostname, hostip, "2h", numOfRecords);
    int result = act->run();
    delete act;

}
