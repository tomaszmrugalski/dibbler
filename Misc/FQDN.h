/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: FQDN.h,v 1.5 2008-08-29 00:07:30 thomson Exp $
 */

#ifndef FQDN_H
#define FQDN_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include "DUID.h"
#include "IPv6Addr.h"

class TFQDN
{
        friend std::ostream& operator<<(std::ostream& out,TFQDN& truc);
public:
    TFQDN();    //Creates any addresses and names
    TFQDN(TDUID *duid_, const std::string& name, bool used);
    TFQDN(TIPv6Addr *addr_, const std::string& name, bool used);
    TFQDN(const std::string& name_, bool used);
    TDUID* getDuid();
    TIPv6Addr* getAddr();
    std::string getName();
    bool isUsed();
    void setUsed(bool used);

 private:
    TDUID *Duid_;
    TIPv6Addr *Addr_;
    std::string Name_;
    bool Used_;
};
#endif
