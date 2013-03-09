/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef STATIONRANGE_H
#define STATIONRANGE_H

#include "IPv6Addr.h"
#include "DUID.h"
#include "SmartPtr.h"

#include <iostream>
#include <iomanip>

class THostRange
{
    friend std::ostream& operator<<(std::ostream& out, THostRange& station);
 public:
    THostRange(SPtr<TDUID> duidl, SPtr<TDUID> duidr);
    THostRange(SPtr<TIPv6Addr> addrl, SPtr<TIPv6Addr> addrR);
    ~THostRange(void);
    bool in(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr) const;
    bool in(SPtr<TIPv6Addr> addr) const;
    bool in(SPtr<TDUID> duid) const;
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
