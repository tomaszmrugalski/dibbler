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
	TStationRange( SmartPtr<TDUID> duidl, SmartPtr<TDUID> duidr);
    TStationRange( SmartPtr<TDUID> duid);
	TStationRange( SmartPtr<TIPv6Addr> addrl, SmartPtr<TIPv6Addr> addrR);
    TStationRange( SmartPtr<TIPv6Addr> addr);
    bool in(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr);
    bool in(SmartPtr<TIPv6Addr> addr);
    bool in(SmartPtr<TDUID> duid);
    SmartPtr<TIPv6Addr> getRandomAddr();
    unsigned long rangeCount();
    //TStationRange(const TStationRange& info);
    
	~TStationRange(void);
private:
    bool isAddrRange;
    SmartPtr<TDUID> DUIDL;
    SmartPtr<TDUID> DUIDR;
    SmartPtr<TIPv6Addr> AddrL;
    SmartPtr<TIPv6Addr> AddrR;
};


#endif;

