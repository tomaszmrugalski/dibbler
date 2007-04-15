/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: AddrAddr.cpp,v 1.10.2.1 2007-04-15 17:52:04 thomson Exp $
 */

#include <iostream>
#include <time.h>

#include "Portable.h"
#include "AddrAddr.h"
#include "DHCPConst.h"
#include "Logger.h"

//

TAddrAddr::TAddrAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid) {
    this->Prefered = pref;
    this->Valid = valid;
    this->Addr=addr;
    this->Timestamp = now();
    this->Tentative = TENTATIVE_UNKNOWN;

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
	  << ">" << x.Addr->getPlain()<< "</AddrAddr>" << std::endl;
    return strum;
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2007-03-04 20:56:44  thomson
 * Large timeout does are not confusing anymore (bug #141), timestamp printed.
 *
 * Revision 1.9  2007-01-27 17:09:31  thomson
 * Negative timeout now works ok.
 *
 * Revision 1.8  2006-11-15 02:58:45  thomson
 * lowlevel-win32.c cleanup, enums added.
 *
 * Revision 1.7  2005/08/03 23:23:35  thomson
 * Minor fix.
 *
 * Revision 1.6  2005/08/03 23:17:11  thomson
 * Minor changes fixed.
 *
 * Revision 1.5  2004/06/04 19:03:46  thomson
 * Resolved warnings with signed/unisigned
 *
 * Revision 1.4  2004/04/10 14:14:30  thomson
 * *** empty log message ***
 *
 * Revision 1.3  2004/03/29 18:53:09  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 */
