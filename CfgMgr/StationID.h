#ifndef STATIONID_H_
#define STATIONID_H_

#include "SmartPtr.h"
#include "IPv6Addr.h"
#include "DUID.h"

#include <iostream>
#include <iomanip>

using namespace std;

class TStationID
{
	friend std::ostream& operator<<(std::ostream& out,TStationID&  station);
public:

	TStationID(SmartPtr<TIPv6Addr> addr);
    TStationID(SmartPtr<TDUID> duid);
    bool operator==(SmartPtr<TIPv6Addr> addr);
    bool operator==(SmartPtr<TDUID> duid);

	//TStationID(const TStationID& info);
	//~TStationID();
private:
    bool isIDAddress;
    SmartPtr<TIPv6Addr> Addr;
    SmartPtr<TDUID> DUID;
};

#endif 
