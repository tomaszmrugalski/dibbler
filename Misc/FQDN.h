#ifndef FQDN_H
#define FQDN_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include "DUID.h"
#include "IPv6Addr.h"
using namespace std;

class TFQDN
{
	friend std::ostream& operator<<(std::ostream& out,TFQDN& truc);
public:
    TFQDN();    //Creates any addresses and names
    TFQDN(TDUID *duid_, string name_,bool used);
    TFQDN(TIPv6Addr *addr_, string name_,bool used);
    TFQDN(string name_,bool used);
    TDUID getDuid();
    TIPv6Addr getAddr();
    string getName();
    bool isUsed();

    TDUID *Duid;
    TIPv6Addr *Addr;
    string Name;
    bool used;
};
#endif
