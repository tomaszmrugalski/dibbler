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
    ~TStationRange(void);
    TStationRange(SPtr<TDUID> duidl, SPtr<TDUID> duidr);
    TStationRange(SPtr<TIPv6Addr> addrl, SPtr<TIPv6Addr> addrR);
    bool in(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr);
    bool in(SPtr<TIPv6Addr> addr);
    bool in(SPtr<TDUID> duid);
    SPtr<TIPv6Addr> getRandomAddr() const;
    SPtr<TIPv6Addr> getRandomPrefix() const;
    unsigned long rangeCount() const;
    SPtr<TIPv6Addr> getAddrL() const;
    SPtr<TIPv6Addr> getAddrR() const;
    int getPrefixLength() const;
    void setPrefixLength(int len);
    void truncate(int minPrefix, int maxPrefix);
 private:
    bool isAddrRange_;
    SPtr<TDUID> DUIDL_;
    SPtr<TDUID> DUIDR_;
    SPtr<TIPv6Addr> AddrL_;
    SPtr<TIPv6Addr> AddrR_;

    int PrefixLength_;
};

#endif
