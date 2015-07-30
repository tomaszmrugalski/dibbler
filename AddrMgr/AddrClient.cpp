/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 licence
 *
 */

#include <iostream>
#include <string>
#include <limits.h>
#include <stdlib.h>
#include "AddrClient.h"
#include "Logger.h"
#include "hex.h"

using namespace std;

/**
 * @brief constructor for creating client
 *
 * constructor used for creating client container. Client
 * contains list of IAs, TAs and PDs.
 *
 * @param duid Client DUID
 *
 */
TAddrClient::TAddrClient(SPtr<TDUID> duid)
    :DUID_(duid), SPI_(0), ReplayDetectionRcvd_(0) {
}

SPtr<TDUID> TAddrClient::getDUID() {
    return DUID_;
}

// --- IA ------------------------------------------------------------


/**
 * @brief rewinds IA list to the beginning
 *
 * rewinds IA list to the beginning
 */
void TAddrClient::firstIA() {
  IAsLst.first();
}

/**
 * @brief returns next IA
 *
 * returns next IA. Make sure to all firstIA() method before calling
 * getIA() for the first time.
 *
 *
 * @return next IA
 */
SPtr<TAddrIA> TAddrClient::getIA() {
    return IAsLst.get();
}

SPtr<TAddrIA> TAddrClient::getIA(unsigned long IAID) {
    SPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return SPtr<TAddrIA>();
}

void TAddrClient::addIA(SPtr<TAddrIA> ia) {
    if (getIA(ia->getIAID()))
    {
        Log(Debug) << "Unable to add IA (iaid=" << ia->getIAID() << "), such IA already exists." << LogEnd;
              return;
    }
    IAsLst.append(ia);
}

/**
 * @brief returns number of IAs in this client
 *
 * returns number of IAs in this client
 *
 * @return number of IAs
 */
int TAddrClient::countIA() {
    return IAsLst.count();
}

bool TAddrClient::delIA(unsigned long IAID) {
    SPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getIAID() == IAID) {
            IAsLst.del();
            return true;
        }
    }
    return false;
}

// --- PD ------------------------------------------------------------

SPtr<TAddrIA> TAddrClient::getPD() {
    return PDLst.get();
}

SPtr<TAddrIA> TAddrClient::getPD(unsigned long IAID) {
    SPtr<TAddrIA> ptr;
    PDLst.first();

    while ( ptr = PDLst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return SPtr<TAddrIA>();
}

void TAddrClient::firstPD() {
    PDLst.first();
}

void TAddrClient::addPD(SPtr<TAddrIA> pd) {
    PDLst.append(pd);
}

int TAddrClient::countPD() {
    return PDLst.count();
}

bool TAddrClient::delPD(unsigned long IAID) {
    SPtr<TAddrIA> ptr;
    PDLst.first();

    while ( ptr = PDLst.get() ) {
        if (ptr->getIAID() == IAID) {
            PDLst.del();
            return true;
        }
    }
    return false;
}

// --- TA ------------------------------------------------------------

SPtr<TAddrIA> TAddrClient::getTA() {
    return TALst.get();
}

SPtr<TAddrIA> TAddrClient::getTA(unsigned long IAID) {
    SPtr<TAddrIA> ptr;
    TALst.first();

    while ( ptr = TALst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return SPtr<TAddrIA>();
}

void TAddrClient::firstTA() {
    TALst.first();
}

void TAddrClient::addTA(SPtr<TAddrIA> ia) {
    TALst.append(ia);
}

int TAddrClient::countTA() {
    return TALst.count();
}

bool TAddrClient::delTA(unsigned long iaid) {
    SPtr<TAddrIA> ptr;
    TALst.first();
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
    SPtr<TAddrIA> ptr;
    unsigned long ts = UINT_MAX;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()==STATE_CONFIGURED) {
            if (ts > ptr->getT1Timeout())
                ts = ptr->getT1Timeout();
        }else if (ptr->getState()==STATE_NOTCONFIGURED){
            ts = 0;
        }
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getT1Timeout())
            ts = ptr->getT1Timeout();
    }
    return ts;
}

unsigned long TAddrClient::getT2Timeout() {
    SPtr<TAddrIA> ptr;
    unsigned long ts = UINT_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getT2Timeout())
            ts = ptr->getT2Timeout();
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getT2Timeout())
            ts = ptr->getT2Timeout();
    }

    return ts;
}

unsigned long TAddrClient::getPrefTimeout() {
    SPtr<TAddrIA> ptr;
    unsigned long ts = UINT_MAX;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getPrefTimeout())
            ts = ptr->getPrefTimeout();
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getPrefTimeout())
            ts = ptr->getPrefTimeout();
    }

    return ts;
}

unsigned long TAddrClient::getValidTimeout() {
    SPtr<TAddrIA> ptr;
    unsigned long ts = UINT_MAX;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ts > ptr->getValidTimeout())
            ts = ptr->getValidTimeout();
    }

    TALst.first();
    while ( ptr = TALst.get() ) {
        if (ts > ptr->getValidTimeout())
            ts = ptr->getValidTimeout();
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
        if (ts > ptr->getValidTimeout())
            ts = ptr->getValidTimeout();
    }

    return ts;
}

unsigned long TAddrClient::getLastTimestamp() {

    unsigned long ts = 0;
    SPtr<TAddrIA> ptr;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ts < ptr->getTimestamp())
            ts = ptr->getTimestamp();
    }

    firstTA();
    while ( ptr = getTA() ) {
        if (ts < ptr->getTimestamp())
            ts = ptr->getTimestamp();
    }


    PDLst.first();
    while ( ptr = PDLst.get() ) {
        if (ts > ptr->getTimestamp())
            ts = ptr->getTimestamp();
    }

    return ts;
}

// --------------------------------------------------------------------
// --- authentication related methods ---------------------------------
// --------------------------------------------------------------------

uint32_t TAddrClient::getSPI() {
    return SPI_;
}

void TAddrClient::setSPI(uint32_t val) {
    SPI_ = val;
}

uint64_t TAddrClient::getReplayDetectionRcvd() {
    return ReplayDetectionRcvd_;
}

void TAddrClient::setReplayDetectionRcvd(uint64_t val) {
    ReplayDetectionRcvd_ = val;
}

void TAddrClient::generateReconfKey() {
    ReconfKey_.resize(16);

    fill_random(&ReconfKey_[0], 16);
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

std::ostream & operator<<(std::ostream & strum, TAddrClient &x)
{
    strum << "  <AddrClient>" << endl;
    if (x.DUID_->getLen())
        strum << "    " << *x.DUID_;
    if (x.DUID_->getLen()==1)
        strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << endl;

    // reconfigure-key
	if (!x.ReconfKey_.empty()) {
		strum << "    <ReconfigureKey length=\"" << x.ReconfKey_.size() << "\">"
	          << hexToText(&x.ReconfKey_[0], x.ReconfKey_.size(), false)
			  << "</ReconfigureKey>" << endl;
	}
	else {
		strum << "    <ReconfigureKey />" << endl;
	}

    strum << "    <!-- " << x.IAsLst.count() << " IA(s) -->" << endl;
    SPtr<TAddrIA> ptr;
    x.IAsLst.first();
    while (ptr = x.IAsLst.get() ) {
        strum << *ptr;
    }

    strum << "    <!-- " << x.TALst.count() << " TA(s) -->" << endl;
    x.TALst.first();
    while (ptr = x.TALst.get() ) {
        strum << *ptr;
    }

    strum << "    <!-- " << x.PDLst.count() << " PD(s) -->" << endl;
    x.PDLst.first();
    while (ptr = x.PDLst.get() ) {
        strum << *ptr;
    }
    strum << "  </AddrClient>" << endl;
    return strum;
}
