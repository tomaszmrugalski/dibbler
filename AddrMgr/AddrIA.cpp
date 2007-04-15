/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: AddrIA.cpp,v 1.18.2.1 2007-04-15 21:23:28 thomson Exp $
 *
 */

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

TAddrIA::TAddrIA(int iface, SmartPtr<TIPv6Addr> addr, SmartPtr<TDUID> duid, 
		 unsigned long T1, unsigned long T2,unsigned long ID) 
{
    this->T1 = T1;
    this->T2 = T2;
    this->IAID = ID;
    this->Tentative = TENTATIVE_UNKNOWN;
    this->State     = STATE_NOTCONFIGURED;
    this->Timestamp = now();
    this->Unicast = false;
    this->setDUID(duid);
    this->Iface = iface;
    if (addr)
        this->setUnicast(addr);
    else
        this->setMulticast();
}

unsigned long TAddrIA::getIAID()
{
    return this->IAID;
}

int TAddrIA::getIface()
{
    return this->Iface;
}

void TAddrIA::setT1(unsigned long T1)
{
    this->T1 = T1;
}

unsigned long TAddrIA::getT1()
{
    return T1;
}

unsigned long TAddrIA::getT2()
{
    return T2;
}

void TAddrIA::setT2(unsigned long T2)
{
    this->T2 = T2;
}

void TAddrIA::addAddr(SmartPtr<TAddrAddr> x)
{
    AddrLst.append(x);
    Tentative = TENTATIVE_UNKNOWN;
}

void TAddrIA::addAddr(SmartPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid)
{
    SmartPtr<TAddrAddr> ptr = new TAddrAddr(addr, pref, valid);
    AddrLst.append(ptr);
    Tentative = TENTATIVE_UNKNOWN;
}

void TAddrIA::addAddr(SmartPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid, int prefix)
{
    SmartPtr<TAddrAddr> ptr = new TAddrAddr(addr, pref, valid, prefix);
    AddrLst.append(ptr);
    Tentative = TENTATIVE_UNKNOWN;
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
    this->SrvAddr=addr;
}

void TAddrIA::setMulticast()
{
    this->Unicast = false;
}

SmartPtr<TIPv6Addr> TAddrIA::getSrvAddr()
{
    if (!this->Unicast) 
        return 0;
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

/** 
 * This function returns TAddrAddr object or 0 if such address is not present
 * 
 * @param addr 
 * 
 * @return 
 */
SmartPtr<TAddrAddr> TAddrIA::getAddr(SmartPtr<TIPv6Addr> addr)
{
    if (!addr)
	return 0;
    AddrLst.first();
    SmartPtr <TAddrAddr> ptrAddr;
    while (ptrAddr = AddrLst.get() ) {
        if ( (*addr)==(*(ptrAddr->get())) )
            return ptrAddr;
    }
    return 0;
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
    return -1;
}

int TAddrIA::getAddrCount()
{
    return this->AddrLst.count();
}

// --------------------------------------------------------------------
// --- prefix related methods -----------------------------------------
// --------------------------------------------------------------------
void TAddrIA::firstPrefix() 
{
    this->PrefixLst.first();
}
SPtr<TAddrPrefix> TAddrIA::getPrefix()
{
    return this->PrefixLst.get();
}

void TAddrIA::addPrefix(SPtr<TAddrPrefix> x)
{
    this->PrefixLst.append(x);
}

void TAddrIA::addPrefix(SPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid,
	       int length)
{
    SmartPtr<TAddrPrefix> ptr = new TAddrPrefix(addr, pref, valid, length);
    PrefixLst.append(ptr);
}

int TAddrIA::getPrefixCount()
{
    return this->PrefixLst.count();
}

bool TAddrIA::delPrefix(SPtr<TAddrPrefix> x)
{
    SmartPtr<TAddrPrefix> ptr;
    PrefixLst.first();

    while (ptr = PrefixLst.get())
    {
	// FIXME: should we compare prefix length, too?
        if (*(ptr->get())==(*x->get())) {
            PrefixLst.del();
            return true;
        }
    }
    return false;
}

bool TAddrIA::delPrefix(SPtr<TIPv6Addr> x)
{
    SmartPtr<TAddrPrefix> ptr;
    PrefixLst.first();

    while (ptr = PrefixLst.get())
    {
	// FIXME: should we compare prefix length, too?
        if (*(ptr->get())==(*x)) {
            PrefixLst.del();
            return true;
        }
    }
    return false;
}

// --------------------------------------------------------------------
// --- time related methods -------------------------------------------
// --------------------------------------------------------------------
unsigned long TAddrIA::getT1Timeout() {
    unsigned long ts, x;
    ts = Timestamp + T1;
    if (ts<Timestamp) { // (Timestamp + T1 overflowed (unsigned long) maximum value
	return DHCPV6_INFINITY;
    }
    
    x  = now();
    if (ts>x)  
        return ts-x;
    else
        return 0;
}

unsigned long TAddrIA::getT2Timeout() {
    unsigned long ts, x;
    ts = Timestamp + T2;
    if (ts<Timestamp) { // (Timestamp + T1 overflowed (unsigned long) maximum value
	return DHCPV6_INFINITY;
    }

    x  = now();
    if (ts>x) 
        return ts-x;
    else 
        return 0;
}

unsigned long TAddrIA::getPrefTimeout() {
    unsigned long ts = ULONG_MAX;

    SmartPtr<TAddrAddr> ptr;
    this->AddrLst.first();

    while (ptr = this->AddrLst.get() ) {
        if (ts > ptr->getPrefTimeout()) 
            ts = ptr->getPrefTimeout();
    }
    return ts;
}

unsigned long TAddrIA::getMaxValidTimeout() {
    unsigned long ts = 0; // should be 0

    SmartPtr<TAddrAddr> ptr;
    this->AddrLst.first();

    while (ptr = this->AddrLst.get() ) {
        if (ts < ptr->getValidTimeout()) 
            ts = ptr->getValidTimeout();
    }
    return ts;
}

unsigned long TAddrIA::getValidTimeout() {
    unsigned long ts = ULONG_MAX;

    SmartPtr<TAddrAddr> ptr;
    this->AddrLst.first();

    while (ptr = this->AddrLst.get() ) {
        if (ts > ptr->getValidTimeout()) 
            ts = ptr->getValidTimeout();
    }
    return ts;
}

unsigned long TAddrIA::getMaxValid() {
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
void TAddrIA::setTimestamp(unsigned long ts)
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

unsigned long TAddrIA::getTentativeTimeout()
{
    unsigned long min = DHCPV6_INFINITY;
    switch (this->getTentative()) 
    {
    case TENTATIVE_YES:
        return 0;
    case TENTATIVE_NO:
        return DHCPV6_INFINITY;
    case TENTATIVE_UNKNOWN:
        SmartPtr <TAddrAddr> ptrAddr;
        AddrLst.first();
        while ( ptrAddr = AddrLst.get() )
        {
            if (ptrAddr->getTentative()==TENTATIVE_UNKNOWN)
                if (min > ptrAddr->getTimestamp()+DADTIMEOUT-now() ) 
                {
                    min = ptrAddr->getTimestamp()+DADTIMEOUT-now();
                }
        }
    }
    return min;
}

// returns Tentative status (TENTATIVE_YES/TENTATIVE_NO/TENTATIVE_UNKNOWN)
enum ETentative TAddrIA::getTentative()
{
    if (Tentative != TENTATIVE_UNKNOWN)
    	return Tentative;
    //if (Timestamp+DADTIMEOUT > now() )
	//    return TENTATIVE_UNKNOWN;

    SmartPtr<TAddrAddr> ptrAddr;
    AddrLst.first();

    bool allChecked = true;
    
    while ( ptrAddr = AddrLst.get() ) {
	switch (ptrAddr->getTentative()) {
	case TENTATIVE_YES:
	    Log(Warning) << "DAD failed. Address " << ptrAddr->get()->getPlain() 
			 << " was detected as tentative." << LogEnd;
	    this->Tentative = TENTATIVE_YES;
	    return TENTATIVE_YES;
	case TENTATIVE_NO:
	    continue;
	case TENTATIVE_UNKNOWN:
        if ( ptrAddr->getTimestamp()+DADTIMEOUT < now() ) 
        {

            switch (is_addr_tentative(NULL, this->Iface, ptrAddr->get()->getPlain()) ) 
            {
                case    LOWLEVEL_TENTATIVE_YES:  
                    ptrAddr->setTentative(TENTATIVE_YES);
                    this->Tentative=TENTATIVE_YES;
                    return TENTATIVE_YES;
                case    LOWLEVEL_TENTATIVE_NO:
                    ptrAddr->setTentative(TENTATIVE_NO);
                    Log(Debug) << "DAD finished successfully. Address " << ptrAddr->get()->getPlain()
			       << " is not tentative." << LogEnd;
                    break;
                default:
                    Log(Error) << "DAD inconclusive. Unable to dermine " << ptrAddr->get()->getPlain() 
                               << " address state. Assuming NOT TENTATIVE." << LogEnd;
                    ptrAddr->setTentative(TENTATIVE_NO);
                    break;
            }
        } 
        else 
            allChecked = false;
	}
    }
    if (allChecked) {
        this->Tentative = TENTATIVE_NO;
	    return TENTATIVE_NO;
    } else {
	    return TENTATIVE_UNKNOWN;
    }
}

void TAddrIA::setTentative()
{
    SmartPtr<TAddrAddr> ptrAddr;
    AddrLst.first();
    Tentative = TENTATIVE_NO;

    while ( ptrAddr = AddrLst.get() ) 
    {
        switch (ptrAddr->getTentative()) 
        {
            case TENTATIVE_YES:
                Tentative = TENTATIVE_YES;
                return;
            case TENTATIVE_NO:
                continue;
            case TENTATIVE_UNKNOWN:
                Tentative = TENTATIVE_UNKNOWN;
                break;
        }
    }
}

/** 
 * stores DNS server address, at which DNSUpdate was performed
 * 
 * @param srvAddr 
 */
void TAddrIA::setFQDNDnsServer(SPtr<TIPv6Addr> srvAddr)
{
    this->fqdnDnsServer = srvAddr;
}

/** 
 * return DNS server address, where the DNSUpdate was performed
 * 
 * 
 * @return 
 */
SPtr<TIPv6Addr> TAddrIA::getFQDNDnsServer()
{
    return this->fqdnDnsServer;
}

/** 
 * stores FQDN information
 * 
 * @param fqdn 
 */
void TAddrIA::setFQDN(SPtr<TFQDN> fqdn)
{
    this->fqdn = fqdn;
}

/** 
 * returns stored FQDN information
 * 
 * @return 
 */
SPtr<TFQDN> TAddrIA::getFQDN()
{
    return this->fqdn;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream & strum,TAddrIA &x) {

    SmartPtr<TAddrAddr> ptr;
    SPtr<TAddrPrefix> prefix;

	strum << "    <AddrIA unicast=\"";
    if (x.Unicast)
	strum << x.SrvAddr->getPlain();
    strum << "\" T1=\"" << x.T1 << "\""
	   << " T2=\"" << x.T2 << "\""
	   << " IAID=\"" << x.IAID << "\""
	   << " state=\"" << StateToString(x.State) 
	   << "\" iface=\"" << x.Iface << "\"" << ">" << endl;
    if (x.getDUID() && x.getDUID()->getLen())
        strum << "      " << *x.DUID;

    // Address list
    x.AddrLst.first();
    while (ptr = x.AddrLst.get()) {
	if (ptr)
	    strum << "      " << *ptr;
    }

    // Prefix list
    x.PrefixLst.first();
    while (prefix = x.PrefixLst.get()) {
	    strum << "      " << *prefix;
    }

    // FQDN
    if (x.fqdnDnsServer) {
	strum << "      <fqdnDnsServer>" << x.fqdnDnsServer->getPlain() << "</fqdnDnsServer>" << endl;
    } else {
	strum << "      <!--<fqdnDnsServer>-->" << endl;
    }
    if (x.fqdn) {
	strum << "      " << *x.fqdn << endl;
    } else {
	strum << "      <!-- <fqdn>-->" << endl;
    }

    strum << "    </AddrIA>" << dec << endl;
    return strum;
}
