/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef FQDN_H
#define FQDN_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include "DUID.h"
#include "IPv6Addr.h"
#include "SmartPtr.h"

class TFQDN
{
        friend std::ostream& operator<<(std::ostream& out,TFQDN& truc);
public:
    TFQDN();    //Creates any addresses and names
    TFQDN(SPtr<TDUID> duid, const std::string& name, bool used);
    TFQDN(SPtr<TIPv6Addr> addr, const std::string& name, bool used);
    TFQDN(const std::string& name, bool used);
    SPtr<TDUID> getDuid();
    SPtr<TIPv6Addr> getAddr();
    std::string getName();
    bool isUsed();
    void setUsed(bool used);

 private:
    SPtr<TDUID> Duid_;
    SPtr<TIPv6Addr> Addr_;
    std::string Name_;
    bool Used_;
};
#endif
