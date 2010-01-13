/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "StationRange.h"
#include "DHCPConst.h"
#include "Logger.h"

TStationRange::TStationRange(void)
{
    
}
TStationRange::TStationRange( SPtr<TDUID> duidl, SPtr<TDUID> duidr)
{
    this->DUIDL=duidl;
    this->DUIDR=duidr;
    this->isAddrRange=false;
}

TStationRange::TStationRange( SPtr<TIPv6Addr> addrl, SPtr<TIPv6Addr> addrr)
{
    this->AddrL=addrl;
    this->AddrR=addrr;
    this->isAddrRange=true;
}

TStationRange::TStationRange( SPtr<TDUID> duid)
{
    this->DUIDL=this->DUIDR=duid;
    this->isAddrRange=false;
}

TStationRange::TStationRange( SPtr<TIPv6Addr> addr)
{
    this->AddrL=this->AddrR=addr;
    this->isAddrRange=true;
}

bool TStationRange::in(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr)
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

bool TStationRange::in(SPtr<TIPv6Addr> addr)
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

bool TStationRange::in(SPtr<TDUID> duid)
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

SPtr<TIPv6Addr> TStationRange::getRandomAddr()
{
    if(isAddrRange)
    {
        SPtr<TIPv6Addr> diff = new TIPv6Addr();
        *diff=(*AddrR)-(*AddrL);
        --(*diff);
        *diff=*diff+*AddrL;
        return diff;
    }
    else
        return 0;
}

SPtr<TIPv6Addr> TStationRange::getRandomPrefix()
{
    if(isAddrRange)
    {
        SPtr<TIPv6Addr> diff = new TIPv6Addr();
        *diff=(*AddrR)-(*AddrL);
        --(*diff);
        *diff=*diff+*AddrL;
        return diff;
    }
    else
        return 0;
}



unsigned long TStationRange::rangeCount()
{
    if(isAddrRange)
    {

        SPtr<TIPv6Addr> diff(new TIPv6Addr());
        *diff=(*AddrR)-(*AddrL);
        char *addr=diff->getAddr();
        for(int i=0;i<12;i++)
        {
            if (addr[i]>0) return DHCPV6_INFINITY;
        }
        unsigned long retVal = addr[12]*256*256*256 + addr[13]*256*256 + addr[14]*256 + addr[15];
	if (retVal!=DHCPV6_INFINITY)
	    return retVal + 1;
        return retVal;
    }
    else
        return 0;
}

int TStationRange::getPrefixLength()
{
    return PrefixLength;
}

void TStationRange::setPrefixLength(int len)
{
    this->PrefixLength = len;
}


TStationRange::~TStationRange(void) {
}

SPtr<TIPv6Addr> TStationRange::getAddrL() {
    return this->AddrL;
}

SPtr<TIPv6Addr> TStationRange::getAddrR() {
    return this->AddrR;
}

void TStationRange::truncate(int minPrefix, int maxPrefix)
{
    if (!isAddrRange) {
	Log(Error) << "Unable to truncace this pool: this is DUID pool, not address pool." << LogEnd;
	return;
    }
    // if the L and R addresses are not at the prefix boundaries, then we are
    // pretty f%%%ed up
    AddrL->truncate(minPrefix, maxPrefix);
    AddrR->truncate(minPrefix, maxPrefix);
}

ostream& operator<<(ostream& out,TStationRange&  range)
{
    if (range.isAddrRange) {
	// address range
        if(range.AddrL&&range.AddrR)
	    out << "      <range type=\"addr\" min=\"" << *range.AddrL 
		<< "\" max=\""  << *range.AddrR << "\"/>" << std::endl;
    } else {
	// DUID range
        if (range.DUIDL&&range.DUIDR) {
	    out << "      <range type=\"duid\">" << std::endl
		<< "         " << *range.DUIDL
		<< "         " << *range.DUIDR
		<< "      </range>" << std::endl;
	}
    }
    return out;
}
