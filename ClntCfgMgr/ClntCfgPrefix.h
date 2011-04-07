/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 *
 *
 */

#ifndef CLNTCFGPREFIX_H
#define CLNTCFGPREFIX_H

#include "DHCPConst.h"
#include "ClntParsGlobalOpt.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include <iostream>
#include <iomanip>

using namespace std;

class TClntCfgPrefix
{
    friend std::ostream& operator<<(std::ostream& out,TClntCfgPrefix& group);
 public:
    TClntCfgPrefix();
    TClntCfgPrefix(SPtr<TIPv6Addr> prefix, unsigned char prefixLength);
    TClntCfgPrefix(SPtr<TIPv6Addr> prefix, unsigned long valid, 
                   unsigned long pref, unsigned char prefixLength);
    ~TClntCfgPrefix();

    SPtr<TIPv6Addr> get();
    unsigned long getValid();
    unsigned long getPref();
    inline unsigned char getLength() { return PrefixLength; }
    void setOptions(SPtr<TClntParsGlobalOpt> opt);
 private:
    SPtr<TIPv6Addr> Prefix;
    unsigned long Valid;
    unsigned long Pref;
    unsigned char PrefixLength;
};

#endif
