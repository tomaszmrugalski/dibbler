#include <iostream>
#include <strstream>
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

long TAddrClient::getT1Timeout() {
    SmartPtr<TAddrIA> ptr;
    long ts = LONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=CONFIGURED)
            continue;
        if (ts > ptr->getT1Timeout())
            ts = ptr->getT1Timeout();
    }
    return ts;
}

long TAddrClient::getT2Timeout() {
    SmartPtr<TAddrIA> ptr;
    long ts = LONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=CONFIGURED)
            continue;
        if (ts > ptr->getT2Timeout())
            ts = ptr->getT2Timeout();
    }
    return ts;
}

long TAddrClient::getPrefTimeout() {
    SmartPtr<TAddrIA> ptr;
    long ts = LONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=CONFIGURED)
            continue;
        if (ts > ptr->getPrefTimeout())
            ts = ptr->getPrefTimeout();
    }
    return ts;
}

long TAddrClient::getValidTimeout() {
    SmartPtr<TAddrIA> ptr;
    long ts = LONG_MAX;
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
    strum << hex << "  <AddrClient DUID=\"";
     if (x.DUID->getLen())
	 strum << *x.DUID;
    strum << "\">" << endl;

    SmartPtr<TAddrIA> ptr;
    x.IAsLst.first();
    while (ptr = x.IAsLst.get() ) {
        strum << *ptr;
    }

    strum << "  </AddrClient>" << dec << logger::endl;
    if (x.DUID->getLen()==1)
	strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << logger::endl;
    return strum;
}
