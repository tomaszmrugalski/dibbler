/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceIface.cpp,v 1.1 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include <iostream>
#include "ClntIfaceIface.h"
#include "Portable.h"
#include "Logger.h"

/*
 * stores informations about interface
 */
TClntIfaceIface::TClntIfaceIface(char * name, int id, unsigned int flags, char* mac, 
				 int maclen, char* llAddr, int llAddrCnt, int hwType)
    :TIfaceIface(name, id, flags, mac, maclen, llAddr, llAddrCnt, hwType) {
    this->DNSServerLst.clear();
}

bool TClntIfaceIface::setDNSServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs) {
    // remove old addresses
    SmartPtr<TIPv6Addr> old, addr;
    this->DNSServerLst.first();
    while (old = this->DNSServerLst.get()) {
	// for each already set server...
	addrs.first();
	bool found = false;
	while (addr = addrs.get()) {
	    // ... check if it's on the new list
	    if ( *addr == *old ) {
		found = true;
		break;
	    }
	}
	if (!found) {
	    // it's not on the new list, so remove it
	    Log(Debug) << "Removing DNS server: " << *old << "on interface "
		       << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
	    dns_del(this->getName(), this->getID(), old->getPlain());
	}
    }

    // add new addresses
    addrs.first();
    while (addr = addrs.get()) {
	this->DNSServerLst.first();
	bool found = false;
	while (old = this->DNSServerLst.get()) {
	    if ( *addr == *old ) {
		found = true;
		break;
	    }
	}
	if (found) {
	    // this address is already set
	    Log(Debug) << "DNS server " << *addr << " is already set on interface "
		       << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
	} else {
	    // it's new address,set it!
	    Log(Debug) << "Setting up DNS server: " << * addr << " on interface "
		       << this->getName() << "/" << this->getID() << "." << LogEnd;
	    dns_add(this->getName(), this->getID(), addr->getPlain());
	    this->DNSServerLst.append(addr);
	}
    }
    this->DNSServerLstAddr = srv;
    this->DNSServerLstDUID = duid;
    this->DNSServerLstTimestamp = now();
    return true;
}

bool TClntIfaceIface::setDomainLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(string) domains) {
    return true;
}

bool TClntIfaceIface::setNTPServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs) {
    return true;
}

bool TClntIfaceIface::setTimezone(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string timezone) {
    return true;
}

bool TClntIfaceIface::setSIPServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs) {
    return true;
}
bool TClntIfaceIface::setSIPDomainLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(string) domains) {
    return true;
}

bool TClntIfaceIface::setFQDN(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string fqdn) {
    return true;
}

bool TClntIfaceIface::setNISServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs) {
    return true;
}
bool TClntIfaceIface::setNISDomain(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string domain) {
    return true;
}

bool TClntIfaceIface::setNISPServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs) {
    return true;
}
bool TClntIfaceIface::setNISPDomain(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string domain) {
    return true;
}

bool TClntIfaceIface::setLifetime(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, unsigned int life) {
    return true;
}


TClntIfaceIface::~TClntIfaceIface() {
    this->removeAllOpts();
}

void TClntIfaceIface::removeAllOpts() {
    SmartPtr<TIPv6Addr> addr;
    this->DNSServerLst.first();
    while (addr = this->DNSServerLst.get()) {
	Log(Debug) << "DNS server " << *addr << " removed from interface "
		   << this->getName() << "/" << this->getID() <<"." << LogEnd;
	this->DNSServerLst.del();
	dns_del(this->getName(), this->getID(), addr->getPlain());
    }
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * just prints important informations (debugging & logging)
 */
ostream & operator <<(ostream & strum, TClntIfaceIface &x) {

    strum << "  <IClntIfaceIface";
    strum << " name=\"" << x.Name << "\"";
    strum << " id=\"" << x.ID << "\"";
    strum << " flags=\"" << x.Flags << "\">" << endl;
    strum << " FIXME" << endl;
    return strum;
}
