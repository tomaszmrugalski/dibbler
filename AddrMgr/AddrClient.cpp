/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: AddrClient.cpp,v 1.9.2.1 2006-02-05 23:38:06 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2004/12/03 20:51:41  thomson
 * Logging issues fixed.
 *
 * Revision 1.8  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.7  2004/09/08 21:22:45  thomson
 * Parser improvements, signed/unsigned issues addressed.
 *
 * Revision 1.6  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
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

TAddrClient::TAddrClient(SmartPtr<TDUID> duid) {
    this->DUID=duid;
}

SmartPtr<TDUID> TAddrClient::getDUID() {
    return this->DUID;
}

// --- IA ------------------------------------------------------------

SmartPtr<TAddrIA> TAddrClient::getIA() {
    return IAsLst.get();
}

SmartPtr<TAddrIA> TAddrClient::getIA(unsigned long IAID) {
    SmartPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return SmartPtr<TAddrIA>();
}

void TAddrClient::firstIA() {
    IAsLst.first();
}

void TAddrClient::addIA(SmartPtr<TAddrIA> ia) {
    IAsLst.append(ia);
}

int TAddrClient::countIA() {
    return IAsLst.count();
}

bool TAddrClient::delIA(unsigned long IAID) {
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

// --- TA ------------------------------------------------------------

SmartPtr<TAddrIA> TAddrClient::getTA() {
    return TALst.get();
}

SmartPtr<TAddrIA> TAddrClient::getTA(unsigned long IAID) {
    SmartPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = TALst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return 0;
}

void TAddrClient::firstTA() {
    TALst.first();
}

void TAddrClient::addTA(SmartPtr<TAddrIA> ia) {
    TALst.append(ia);
}

int TAddrClient::countTA() {
    return TALst.count();
}

bool TAddrClient::delTA(unsigned long iaid) {
    SmartPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = TALst.get() ) {
        if (ptr->getIAID() == iaid) {
            TALst.del();
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
    unsigned long ts = ULONG_MAX;
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
    unsigned long ts = ULONG_MAX;
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
    unsigned long ts = ULONG_MAX;
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
    unsigned long ts = ULONG_MAX;
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
	strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << endl;
    strum << "  <AddrClient>" << endl;
    if (x.DUID->getLen())
	strum << "    " << *x.DUID;

    strum << "    <!-- " << x.IAsLst.count() << " IA(s) -->" << endl;
    SmartPtr<TAddrIA> ptr;
    x.IAsLst.first();
    while (ptr = x.IAsLst.get() ) {
        strum << *ptr;
    }

    strum << "    <!-- " << x.TALst.count() << " TA(s) -->" << endl;
    x.TALst.first();
    while (ptr = x.TALst.get() ) {
        strum << *ptr;
    }

    strum << "  </AddrClient>" << endl;
    if (x.DUID->getLen()==1)
	strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << endl;
    return strum;
}
