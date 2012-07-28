/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "HostID.h"

#include <iostream>
#include <iomanip>

using namespace std;

THostID::THostID(SPtr<TIPv6Addr> addr)
{
    this->Addr=addr;
    isIDAddress=true;
}

THostID::THostID(SPtr<TDUID> duid)
{
    this->DUID=duid;
    isIDAddress=false;
}

bool THostID::operator==(SPtr<TIPv6Addr> addr)
{   
    if (!isIDAddress)
        return false;
    return *addr==*Addr;
}

bool THostID::operator==(SPtr<TDUID> duid)
{
    if (isIDAddress)
        return false;
    return *duid==*DUID;
}

ostream& operator<<(ostream& out,THostID&  station)
{
    if (station.DUID) {
        out<<*station.DUID;
    } else {
	out << "<Addr>" << *station.Addr << "</Addr>" << endl;
    }
    return out;
}
