/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

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

	TStationID(SPtr<TIPv6Addr> addr);
    TStationID(SPtr<TDUID> duid);
    bool operator==(SPtr<TIPv6Addr> addr);
    bool operator==(SPtr<TDUID> duid);

	//TStationID(const TStationID& info);
	//~TStationID();
private:
    bool isIDAddress;
    SPtr<TIPv6Addr> Addr;
    SPtr<TDUID> DUID;
};

#endif 
