/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntCfgAddr.h,v 1.6 2008-08-29 00:07:27 thomson Exp $
 *
 */

class TClntCfgAddr;
#ifndef CLNTCFGADDR_H
#define CLNTCFGADDR_H

#include "DHCPConst.h"
#include "ClntParsGlobalOpt.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include <iostream>
#include <iomanip>

using namespace std;

class TClntCfgAddr
{
    friend std::ostream& operator<<(std::ostream& out,TClntCfgAddr& group);
 public:
    TClntCfgAddr();
    TClntCfgAddr(SmartPtr<TIPv6Addr> addr);
    TClntCfgAddr(SmartPtr<TIPv6Addr> addr,long valid,long pref);
    ~TClntCfgAddr();

    SmartPtr<TIPv6Addr> get();
    unsigned long getValid();
    unsigned long getPref();
    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);
 private:
    SmartPtr<TIPv6Addr> Addr;
    unsigned long Valid;
    unsigned long Pref;
};

#endif
