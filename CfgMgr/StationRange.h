/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: StationRange.h,v 1.5 2006-10-06 00:30:46 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004-12-07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.3  2004/07/05 00:12:29  thomson
 * Lots of minor changes.
 *
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

using namespace std;

class TStationRange
{
    friend ostream& operator<<(ostream& out,TStationRange&  station);
 public:
    TStationRange(void);
    ~TStationRange(void);
    TStationRange( SmartPtr<TDUID> duidl, SmartPtr<TDUID> duidr);
    TStationRange( SmartPtr<TDUID> duid);
    TStationRange( SmartPtr<TIPv6Addr> addrl, SmartPtr<TIPv6Addr> addrR);
    TStationRange( SmartPtr<TIPv6Addr> addr);
    bool in(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr);
    bool in(SmartPtr<TIPv6Addr> addr);
    bool in(SmartPtr<TDUID> duid);
    SmartPtr<TIPv6Addr> getRandomAddr();
    SmartPtr<TIPv6Addr> getRandomPrefix();
    unsigned long rangeCount();
    SmartPtr<TIPv6Addr> getAddrL();
    SmartPtr<TIPv6Addr> getAddrR();
 private:
    bool isAddrRange;
    SmartPtr<TDUID> DUIDL;
    SmartPtr<TDUID> DUIDR;
    SmartPtr<TIPv6Addr> AddrL;
    SmartPtr<TIPv6Addr> AddrR;
};

#endif
