#include "StationID.h"

#include <iostream>
#include <iomanip>

using namespace std;

TStationID::TStationID(SmartPtr<TIPv6Addr> addr)
{
    this->Addr=addr;
    isIDAddress=true;
}

TStationID::TStationID(SmartPtr<TDUID> duid)
{
    this->DUID=duid;
    isIDAddress=false;
}

bool TStationID::operator==(SmartPtr<TIPv6Addr> addr)
{   
    if (!isIDAddress)
        return false;
    return *addr==*Addr;
}

bool TStationID::operator==(SmartPtr<TDUID> duid)
{
    if (isIDAddress)
        return false;
    return *duid==*DUID;
}

/*TStationID::TStationID(const TStationID& info)
{
	if (info.DUID)
	{
		DUIDlength=info.DUIDlength;
		DUID=new char[info.DUIDlength];
		memcpy(DUID,info.DUID,DUIDlength);
	}
	else
	{
		DUID=NULL;
		memcpy(Addr,info.Addr,16);
	}
}

TStationID::~TStationID()
{
	if (DUID) 
		delete DUID;	
}*/

ostream& operator<<(ostream& out,TStationID&  station)
{
    if (station.DUID)
    {
		out<<"DUID:";
        out<<*station.DUID;
        
    }
	else
	{
		out << "<Addr>";
        out << *station.Addr;
		out << "</Addr>" << endl;
	}
	return out;
}
