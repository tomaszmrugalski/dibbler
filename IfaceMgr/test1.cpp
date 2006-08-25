#include <iostream>
#include "DNSUpdate.h"

using namespace std;
int main() {

    int numOfRecords = 4;

    DNSUpdate *act = new DNSUpdate("2000::1", "fqdn.example.com.", "fqdn", "2000::123", "2h", numOfRecords);
    int result = act->run();
    delete act;

}
