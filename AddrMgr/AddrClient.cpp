/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: AddrClient.cpp,v 1.6 2004-06-20 19:29:23 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2004/06/04 19:03:46  thomson
 * Resolved warnings with signed/unisigned
 *
 * Revision 1.4  2004/03/29 18:53:09  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 * Revision 1.3  2004/03/28 19:57:59  thomson
 * no message
 *
 */

#include <iostream>
#include <string>
#include <limits.h>
#include "AddrClient.h"
#include "Logger.h"

TAddrClient::TAddrClient(SmartPtr<TDUID> duid)
{
    this->DUID=duid;
}

SmartPtr<TDUID> TAddrClient::getDUID()
{
    return this->DUID;
}

SmartPtr<TAddrIA> TAddrClient::getIA()
{
    return IAsLst.get();
}

SmartPtr<TAddrIA> TAddrClient::getIA(int IAID)
{
    SmartPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return SmartPtr<TAddrIA>();
}

void TAddrClient::firstIA()
{
    IAsLst.first();
}

void TAddrClient::addIA(SmartPtr<TAddrIA> ia)
{
    IAsLst.append(ia);
    //    ia->setDUID(this->DUID,this->DUIDlen);
}

int TAddrClient::countIA()
{
    return IAsLst.count();
}

bool TAddrClient::delIA(long IAID)
{
    SmartPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getIAID() == IAID) {
            IAsLst.del();
            return true;
        }
    }
    return false;
}

// --------------------------------------------------------------------
// --- time related methods -------------------------------------------
// --------------------------------------------------------------------

unsigned long TAddrClient::getT1Timeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = LONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=CONFIGURED)
            continue;
        if (ts > ptr->getT1Timeout())
            ts = ptr->getT1Timeout();
    }
    return ts;
}

unsigned long TAddrClient::getT2Timeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = LONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=CONFIGURED)
            continue;
        if (ts > ptr->getT2Timeout())
            ts = ptr->getT2Timeout();
    }
    return ts;
}

unsigned long TAddrClient::getPrefTimeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = LONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=CONFIGURED)
            continue;
        if (ts > ptr->getPrefTimeout())
            ts = ptr->getPrefTimeout();
    }
    return ts;
}

unsigned long TAddrClient::getValidTimeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = LONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ts > ptr->getValidTimeout())
            ts = ptr->getValidTimeout();
    }
    return ts;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream & strum,TAddrClient &x) 
{
    if (x.DUID->getLen()==1)
	strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << logger::endl;
    strum << "  <AddrClient>" << std::endl;
    if (x.DUID->getLen())
	strum << "    " << *x.DUID;
    
    SmartPtr<TAddrIA> ptr;
    x.IAsLst.first();
    while (ptr = x.IAsLst.get() ) {
        strum << *ptr;
    }

    strum << "  </AddrClient>" << logger::endl;
    if (x.DUID->getLen()==1)
	strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << logger::endl;
    return strum;
}
