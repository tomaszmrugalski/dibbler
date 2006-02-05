/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: AddrIA.cpp,v 1.10.2.1 2006-02-05 23:38:06 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2005/02/01 00:57:36  thomson
 * no message
 *
 * Revision 1.9  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.8  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.7  2004/09/07 22:02:32  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 * Revision 1.6  2004/06/21 23:08:48  thomson
 * Minor fixes.
 *
 * Revision 1.5  2004/06/04 19:03:46  thomson
 * Resolved warnings with signed/unisigned
 *
 * Revision 1.4  2004/03/29 22:06:49  thomson
 * 0.1.1 version
 *
 * Revision 1.3  2004/03/29 18:53:09  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
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
    Tentative = DONTKNOWYET;
}

void TAddrIA::addAddr(SmartPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid)
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
// --- time related methods -------------------------------------------
// --------------------------------------------------------------------
unsigned long TAddrIA::getT1Timeout() {
    unsigned long ts;
    ts = (this->Timestamp) + (this->T1) - now();
    if (ts>0) 
        return ts;
    else 
        return 0;
}

unsigned long TAddrIA::getT2Timeout() {
    unsigned long ts;
    ts = (this->Timestamp) + (this->T2) - now();
    if (ts>0) 
        return ts;
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
    unsigned long ts = 0;

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
                if (min > ptrAddr->getTimestamp()+DADTIMEOUT-now() ) 
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
	    Log(Warning) << "DAD failed. Address " << ptrAddr->get()->getPlain() 
			 << " was detected as tentative." << LogEnd;
	    this->Tentative = YES;
	    return YES;
	case NO:
	    continue;
	case DONTKNOWYET:
        if ( ptrAddr->getTimestamp()+DADTIMEOUT < now() ) 
        {

            switch (is_addr_tentative(NULL, this->Iface, ptrAddr->get()->getPlain()) ) 
            {
                case    1:  
                    ptrAddr->setTentative(YES);
                    this->Tentative=YES;
                    return YES;
                case    0:
                    ptrAddr->setTentative(NO);
                    Log(Debug) << "DAD finished successfully. Address " << ptrAddr->get()->getPlain()
			       << " is not tentative." << LogEnd;
                    break;
                default:
                    Log(Error) << "DAD inconclusive. Unable to dermine " << ptrAddr->get()->getPlain() 
                               << " address state. Assuming NOT TENTATIVE." << LogEnd;
                    ptrAddr->setTentative(NO);
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

	strum << "    <AddrIA unicast=\"";
    if (x.Unicast)
	strum << x.SrvAddr->getPlain();
    strum << "\"" << endl;
    
    strum  << "            T1=\"" << x.T1 << "\""
	   << " T2=\"" << x.T2 << "\""
	   << " IAID=\"" << x.IAID << "\""
	   << " state=\"" << StateToString(x.State) 
	   << "\" iface=\"" << x.Iface << "\"" << ">" << endl;
    if (x.getDUID() && x.getDUID()->getLen())
        strum << "      " << *x.DUID;

    x.AddrLst.first();

    while (ptr = x.AddrLst.get()) {
	if (ptr)
	    strum << "      " << *ptr;
    }
    strum << "    </AddrIA>" << dec << endl;
    return strum;
}
