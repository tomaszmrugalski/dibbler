/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "StationID.h"

#include <iostream>
#include <iomanip>

using namespace std;

TStationID::TStationID(SPtr<TIPv6Addr> addr)
{
    this->Addr=addr;
    isIDAddress=true;
}

TStationID::TStationID(SPtr<TDUID> duid)
{
    this->DUID=duid;
    isIDAddress=false;
}

bool TStationID::operator==(SPtr<TIPv6Addr> addr)
{   
    if (!isIDAddress)
        return false;
    return *addr==*Addr;
}

bool TStationID::operator==(SPtr<TDUID> duid)
{
    if (isIDAddress)
        return false;
    return *duid==*DUID;
}

ostream& operator<<(ostream& out,TStationID&  station)
{
    if (station.DUID) {
        out<<*station.DUID;
    } else {
	out << "<Addr>" << *station.Addr << "</Addr>" << endl;
    }
    return out;
}
