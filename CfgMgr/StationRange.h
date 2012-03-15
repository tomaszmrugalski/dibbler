/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TStationRange;
#ifndef STATIONRANGE_H
#define STATIONRANGE_H

#include "IPv6Addr.h"
#include "DUID.h"
#include "SmartPtr.h"

#include <iostream>
#include <iomanip>

class TStationRange
{
    friend std::ostream& operator<<(std::ostream& out, TStationRange& station);
 public:
    TStationRange(void);
    ~TStationRange(void);
    TStationRange(SPtr<TDUID> duidl, SPtr<TDUID> duidr);
    TStationRange(SPtr<TDUID> duid);
    TStationRange(SPtr<TIPv6Addr> addrl, SPtr<TIPv6Addr> addrR);
    TStationRange(SPtr<TIPv6Addr> addr);
    bool in(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr);
    bool in(SPtr<TIPv6Addr> addr);
    bool in(SPtr<TDUID> duid);
    SPtr<TIPv6Addr> getRandomAddr();
    SPtr<TIPv6Addr> getRandomPrefix();
    unsigned long rangeCount();
    SPtr<TIPv6Addr> getAddrL();
    SPtr<TIPv6Addr> getAddrR();
    int getPrefixLength();
    void setPrefixLength(int len);
    void truncate(int minPrefix, int maxPrefix);
 private:
    bool isAddrRange;
    SPtr<TDUID> DUIDL;
    SPtr<TDUID> DUIDR;
    SPtr<TIPv6Addr> AddrL;
    SPtr<TIPv6Addr> AddrR;

    int PrefixLength;
};

#endif
