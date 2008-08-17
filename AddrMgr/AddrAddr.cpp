/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: AddrAddr.cpp,v 1.13 2008-08-17 22:41:41 thomson Exp $
 */

#include <iostream>
#include <time.h>

#include "Portable.h"
#include "AddrAddr.h"
#include "DHCPConst.h"
#include "Logger.h"

TAddrAddr::TAddrAddr(SPtr<TIPv6Addr> addr, long pref, long valid) {
    this->Prefered = pref;
    this->Valid = valid;
    this->Addr=addr;
    this->Timestamp = now();
    this->Tentative = TENTATIVE_UNKNOWN;
    this->Prefix = 128;

    if (pref>valid) {
	Log(Warning) << "Trying to store " << this->Addr->getPlain() << " with prefered(" << pref << ")>valid("
		     << valid << ") lifetimes." << LogEnd;
    }
}

TAddrAddr::TAddrAddr(SPtr<TIPv6Addr> addr, long pref, long valid, int prefix) {
    this->Prefered = pref;
    this->Valid = valid;
    this->Addr=addr;
    this->Timestamp = now();
    this->Tentative = TENTATIVE_UNKNOWN;
    this->Prefix = prefix;

    if (pref>valid) {
	Log(Warning) << "Trying to store " << this->Addr->getPlain() << " with prefered(" << pref << ")>valid("
		     << valid << ") lifetimes." << LogEnd;
    }
}

unsigned long TAddrAddr::getPref() {
    return Prefered;
}

unsigned long TAddrAddr::getValid() {
    return Valid;
}

int TAddrAddr::getPrefix()
{
    return Prefix;
}

SmartPtr<TIPv6Addr> TAddrAddr::get() {
    return Addr;
}

// return Prefered time left
unsigned long TAddrAddr::getPrefTimeout()
{
    unsigned long ts = Timestamp + Prefered;
    unsigned long x  = now();
    if (ts<Timestamp) { // (Timestamp + T1 overflowed (unsigned long) maximum value
	return DHCPV6_INFINITY;
    }
    if (ts>x) 
        return ts-x;
    else 
        return 0;
}

// return Valid time left
unsigned long TAddrAddr::getValidTimeout()
{
    unsigned long ts = Timestamp + Valid;
    unsigned long x  = now();
    if (ts<Timestamp) { // (Timestamp + T1 overflowed (unsigned long) maximum value
	return DHCPV6_INFINITY;
    }

    if (ts>x) 
        return ts-x;
    else 
        return 0;
}

// return timestamp
long TAddrAddr::getTimestamp()
{
    return this->Timestamp;
}

// set timestamp
void TAddrAddr::setTimestamp(long ts)
{
    this->Timestamp = ts;
}

void TAddrAddr::setTentative(enum ETentative state)
{
    this->Tentative = state;
}

void TAddrAddr::setPref(unsigned long pref)
{
    this->Prefered = pref;
}

void TAddrAddr::setValid(unsigned long valid)
{
    this->Valid = valid;
}

// set timestamp
void TAddrAddr::setTimestamp()
{
    this->Timestamp = now();
}

enum ETentative TAddrAddr::getTentative()
{
    return Tentative;
}

ostream & operator<<(ostream & strum,TAddrAddr &x) {
    strum << "<AddrAddr"
	
	  << " timestamp=\"" << x.Timestamp << "\""
	  << " pref=\"" << x.Prefered <<"\""
	  << " valid=\"" << x.Valid <<  "\""
	  << " prefix=\"" << x.Prefix << "\""
	  << ">" << x.Addr->getPlain()<< "</AddrAddr>" << std::endl;
    return strum;
}

