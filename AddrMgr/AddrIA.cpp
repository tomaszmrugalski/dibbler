#include <limits.h>
#include "Portable.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "Logger.h"
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

TAddrIA::TAddrIA(int iface, SmartPtr<TIPv6Addr> addr, SmartPtr<TDUID> duid, long T1, long T2,long ID) 
{
    this->T1 = T1;
    this->T2 = T2;
    this->IAID = ID;
    this->Tentative = DONTKNOWYET;
    this->State = NOTCONFIGURED;
    this->Timestamp = now();
    this->Unicast = false;
    this->setDUID(duid);
    this->Iface = iface;
    if (addr)
        this->setUnicast(addr);
    else
        this->setMulticast();
}

long TAddrIA::getIAID()
{
    return this->IAID;
}

int TAddrIA::getIface()
{
    return this->Iface;
}

void TAddrIA::setT1(long T1)
{
    this->T1 = T1;
}

long TAddrIA::getT1()
{
    return T1;
}

long TAddrIA::getT2()
{
    return T2;
}

void TAddrIA::setT2(long T2)
{
    this->T2 = T2;
}

void TAddrIA::addAddr(SmartPtr<TAddrAddr> x)
{
    AddrLst.append(x);
    Tentative = DONTKNOWYET;
}

void TAddrIA::addAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid)
{
    SmartPtr<TAddrAddr> ptr = new TAddrAddr(addr, pref, valid);
    AddrLst.append(ptr);
    Tentative = DONTKNOWYET;
}

enum EState TAddrIA::getState()
{
    return this->State;
}

void TAddrIA::setState(enum EState state)
{
    this->State = state;
}

TAddrIA::~TAddrIA()
{
}

// --------------------------------------------------------------------
// --- contact with server using Unicast/Multicast --------------------
// --------------------------------------------------------------------

void TAddrIA::setUnicast(SmartPtr<TIPv6Addr> addr)
{
    this->Unicast = true;
    //memcpy(this->SrvAddr,addr,16);
    this->SrvAddr=addr;
}

void TAddrIA::setMulticast()
{
    this->Unicast = false;
}

SmartPtr<TIPv6Addr> TAddrIA::getSrvAddr()
{
    if (!this->Unicast) 
        return SmartPtr<TIPv6Addr> ();
    else
        return SrvAddr;
}
// --------------------------------------------------------------------
// --- server's DUID --------------------------------------------------
// --------------------------------------------------------------------

void TAddrIA::setDUID(SmartPtr<TDUID> duid)
{
    this->DUID = duid;
}

SmartPtr<TDUID> TAddrIA::getDUID()
{
    return DUID;
}
 
// --------------------------------------------------------------------
// --- list related methods -------------------------------------------
// --------------------------------------------------------------------
void TAddrIA::firstAddr()
{
    AddrLst.first();
}

// returns next address
SmartPtr<TAddrAddr> TAddrIA::getAddr()
{
    return AddrLst.get();
}

// returns AddrAddr object 
SmartPtr<TAddrAddr> TAddrIA::getAddr(SmartPtr<TIPv6Addr> addr)
{
    AddrLst.first();
    SmartPtr <TAddrAddr> ptrAddr;
    while (ptrAddr = AddrLst.get() ) {
        if ( (*addr)==(*(ptrAddr->get())) )
            return ptrAddr;
    }
    return SmartPtr<TAddrAddr>();
}

int TAddrIA::countAddr()
{
    return AddrLst.count();
}

int TAddrIA::delAddr(SmartPtr<TIPv6Addr> addr)
{
    SmartPtr< TAddrAddr> ptr;
    AddrLst.first();

    while (ptr = AddrLst.get())
    {
        if (*(ptr->get())==(*addr)) {
            AddrLst.del();
            return 0;
        }
    }
    return 0; //FIXME
}

int TAddrIA::getAddrCount()
{
    return this->AddrLst.count();
}

// --------------------------------------------------------------------
// --- time related methods -------------------------------------------
// --------------------------------------------------------------------
long TAddrIA::getT1Timeout() {
    long ts;
    ts = (this->Timestamp) + (this->T1) - now();
    if (ts>0) 
        return ts;
    else 
        return 0;
}

long TAddrIA::getT2Timeout() {
    long ts;
    ts = (this->Timestamp) + (this->T2) - now();
    if (ts>0) 
        return ts;
    else 
        return 0;
}

long TAddrIA::getPrefTimeout() {
    unsigned long ts = LONG_MAX;

    SmartPtr<TAddrAddr> ptr;
    this->AddrLst.first();

    while (ptr = this->AddrLst.get() ) {
        if (ts > ptr->getPrefTimeout()) 
            ts = ptr->getPrefTimeout();
    }
    return ts;
}

long TAddrIA::getMaxValidTimeout() {
    unsigned long ts = 0;

    SmartPtr<TAddrAddr> ptr;
    this->AddrLst.first();

    while (ptr = this->AddrLst.get() ) {
        if (ts < ptr->getValidTimeout()) 
            ts = ptr->getValidTimeout();
    }
    return ts;
}

long TAddrIA::getValidTimeout() {
    unsigned long ts = LONG_MAX;

    SmartPtr<TAddrAddr> ptr;
    this->AddrLst.first();

    while (ptr = this->AddrLst.get() ) {
        if (ts > ptr->getValidTimeout()) 
            ts = ptr->getValidTimeout();
    }
    return ts;
}

long TAddrIA::getMaxValid() {
    unsigned long ts = 0;

    SmartPtr<TAddrAddr> ptr;
    this->AddrLst.first();

    while (ptr = this->AddrLst.get() ) {
        if (ts < ptr->getValidTimeout()) 
            ts = ptr->getValidTimeout();
    }
    return ts;
}

// set timestamp
void TAddrIA::setTimestamp(long ts)
{
    this->Timestamp = ts;
    SmartPtr<TAddrAddr> ptr;
    AddrLst.first();
    while (ptr = AddrLst.get() )
    {
        ptr->setTimestamp(ts);
    }
}

void TAddrIA::setTimestamp() {
    this->setTimestamp(now());
}

void TAddrIA::addClntAddr(int iface, SmartPtr<TIPv6Addr> addr, long pref,long valid)
{
}

unsigned long TAddrIA::getTentativeTimeout()
{
    unsigned long min = DHCPV6_INFINITY;
    switch (this->getTentative()) 
    {
    case YES:
        return 0;
    case NO:
        return DHCPV6_INFINITY;
    case DONTKNOWYET:
        SmartPtr <TAddrAddr> ptrAddr;
        AddrLst.first();
        while ( ptrAddr = AddrLst.get() )
        {
            if (ptrAddr->getTentative()==DONTKNOWYET)
                if (min > ptrAddr->getTimestamp()+DADTIMEOUT-now()) 
                {
                    min = ptrAddr->getTimestamp()+DADTIMEOUT-now();
                }
        }
    }
    return min;
}

// returns Tentative status (YES/NO/DONTKNOWYET)
enum ETentative TAddrIA::getTentative()
{
    if (Tentative != DONTKNOWYET)
    	return Tentative;
    //if (Timestamp+DADTIMEOUT > now() )
	//    return DONTKNOWYET;

    SmartPtr<TAddrAddr> ptrAddr;
    AddrLst.first();

    bool allChecked = true;
    
    while ( ptrAddr = AddrLst.get() ) {
	switch (ptrAddr->getTentative()) {
	case YES:
	    this->Tentative = YES;
	    return YES;
	case NO:
	    continue;
	case DONTKNOWYET:
        if (ptrAddr->getTimestamp()+DADTIMEOUT < now() ) 
        {

            switch (is_addr_tentative(NULL, this->Iface, ptrAddr->get()->getPlain()) ) 
            {
                case    1:  
                    ptrAddr->setTentative(YES);
                    this->Tentative=YES;
                    return YES;
                case    0:
                    ptrAddr->setTentative(NO);
                    break;
                default:
                    clog<<logger::logError<<"No such a address on interface "
                        <<*ptrAddr->get()<<logger::endl;
                    break;
            }
        } 
        else 
            allChecked = false;

	}
    }
    if (allChecked) {
	this->Tentative = NO;
	return NO;
    } else {
	return DONTKNOWYET;
    }
}

void TAddrIA::setTentative()
{
    SmartPtr<TAddrAddr> ptrAddr;
    AddrLst.first();
    Tentative = NO;

    while ( ptrAddr = AddrLst.get() ) 
    {
        switch (ptrAddr->getTentative()) 
        {
            case YES:
                Tentative = YES;
                return;
            case NO:
                continue;
            case DONTKNOWYET:
                Tentative = DONTKNOWYET;
                break;
        }
    }
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream & strum,TAddrIA &x) {

    SmartPtr<TAddrAddr> ptr;

    strum << "    <AddrIA DUID=\"";
    if (x.getDUID() && x.getDUID()->getLen())
        strum << *x.DUID;
    strum << "\"" << logger::endl;

    strum << "            unicast=\"";
    if (x.Unicast)
	strum << x.SrvAddr->getPlain();
    strum << "\"" << logger::endl;
    
    strum  << "            T1=\"" << x.T1 << "\""
	   << " T2=\"" << x.T2 << "\""
	   << " IAID=\"" << x.IAID << "\""
	   << " state=\"" << x.State << "\""
	   << " iface=\"" << x.Iface << "\""
	   << ">" << endl;

    x.AddrLst.first();

    while (ptr = x.AddrLst.get()) {
	if (ptr)
	    strum << "      " << *ptr;
    }
    strum << "    </AddrIA>" << dec << endl;
    return strum;
}
