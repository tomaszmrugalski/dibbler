#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "StationRange.h"
#include "DHCPConst.h"

TStationRange::TStationRange(void)
{
    
}
TStationRange::TStationRange( SmartPtr<TDUID> duidl, SmartPtr<TDUID> duidr)
{
    this->DUIDL=duidl;
    this->DUIDR=duidr;
    this->isAddrRange=false;
}

TStationRange::TStationRange( SmartPtr<TIPv6Addr> addrl, SmartPtr<TIPv6Addr> addrr)
{
    this->AddrL=addrl;
    this->AddrR=addrr;
    this->isAddrRange=true;
}

TStationRange::TStationRange( SmartPtr<TDUID> duid)
{
    this->DUIDL=this->DUIDR=duid;
    this->isAddrRange=false;
}

TStationRange::TStationRange( SmartPtr<TIPv6Addr> addr)
{
    this->AddrL=this->AddrR=addr;
    this->isAddrRange=true;
}

bool TStationRange::in(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr)
{
    if (isAddrRange)
    {
        if ((*addr<=*AddrL)&&(*AddrL<=*addr)) return true;
        if ((*addr<=*AddrR)&&(*AddrR<=*addr)) return true;
        if (*addr<=*AddrL) return false;
        if (*AddrR<=*addr) return false;
    }
    else
    {
        if (*duid<=*DUIDL) return false;
        if (*DUIDR<=*duid) return false;
    }
    return true;
}

bool TStationRange::in(SmartPtr<TIPv6Addr> addr)
{
    if (isAddrRange)
    {
        if ((*addr<=*AddrL)&&(*AddrL<=*addr)) return true;
        if ((*addr<=*AddrR)&&(*AddrR<=*addr)) return true;
        if (*addr<=*AddrL) return false;
        if (*AddrR<=*addr) return false;
	return true;
    } else
        return false;
}

bool TStationRange::in(SmartPtr<TDUID> duid)
{
    if (isAddrRange)
        return false;
    else
    {
        if (*duid<=*DUIDL) return false;
        if (*DUIDR<=*duid) return false;
    }
    return true;
}

SmartPtr<TIPv6Addr> TStationRange::getRandomAddr()
{
    if(isAddrRange)
    {

        SmartPtr<TIPv6Addr> diff(new TIPv6Addr());
        *diff=(*AddrR)-(*AddrL);
        --(*diff);
        *diff=*diff+*AddrL;
        return diff;
    }
    else
        return new TIPv6Addr();
}


unsigned long TStationRange::rangeCount()
{
    if(isAddrRange)
    {

        SmartPtr<TIPv6Addr> diff(new TIPv6Addr());
        *diff=(*AddrR)-(*AddrL);
        char *addr=diff->getAddr();
        for(int i=0;i<12;i++)
        {
            if (addr[i]>0) return DHCPV6_INFINITY;
        }
        unsigned long retVal=ntohl(*(long*)(addr+12))+1;
        return retVal;
    }
    else
        return 0;
}

TStationRange::~TStationRange(void)
{
}

ostream& operator<<(ostream& out,TStationRange&  range)
{
    if (range.isAddrRange) {
	// address range
        if(range.AddrL&&range.AddrR)
	    out << "      <range min=\"" << *range.AddrL << "\" max=\"" 
		<< *range.AddrR << "\"/>" << std::endl;
    } else {
	// DUID range
        if (range.DUIDL&&range.DUIDR)
	    out << "      <range min=\"" << *range.DUIDL << "\" max=\"" 
		<< *range.DUIDR << "\"/>" << std::endl;
    }
    return out;
}
