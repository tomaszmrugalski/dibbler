/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceIface.cpp,v 1.2 2004-10-27 22:07:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
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
    this->DomainLst.clear();
    this->NTPServerLst.clear();
    this->Timezone = "";

    this->LifetimeTimeout = DHCPV6_INFINITY;
    this->LifetimeTimestamp = now();
}

/*
 * this method returns timeout to nearest option renewal
 */
unsigned int TClntIfaceIface::getTimeout() {
    if (this->LifetimeTimeout == DHCPV6_INFINITY)
	return DHCPV6_INFINITY;
    unsigned int current = now();
    if (current > this->LifetimeTimestamp+this->LifetimeTimeout)
	return 0;
    return this->LifetimeTimestamp+this->LifetimeTimeout-current;
}

bool TClntIfaceIface::setDNSServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, 
				      List(TIPv6Addr) addrs) {
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
	    Log(Debug) << "Removing DNS server: " << *old << " on interface "
		       << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
	    dns_del(this->getName(), this->getID(), old->getPlain());
	    this->DNSServerLst.del();
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
    this->LifetimeTimeout = life;
    this->LifetimeTimestamp = now();
    if (life == DHCPV6_INFINITY)
	Log(Info) << "Granted options are parmanent (lifetime = INFINITY)" << LogEnd;
    Log(Info) << "Next option renewal in " << life << " seconds ." << LogEnd;
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
    char buf[48];
    SmartPtr<TIPv6Addr> addr;
    SmartPtr<string> str;

    strum << "  <ClntIfaceIface";
    strum << " name=\"" << x.Name << "\"";
    strum << " id=\"" << x.ID << "\"";
    strum << " flags=\"" << x.Flags << "\">" << endl;
    strum << "    <!-- " << x.LLAddrCnt << " link scoped addrs -->" << endl;

    for (int i=0; i<x.LLAddrCnt; i++) {
	inet_ntop6(x.LLAddr+i*16,buf);
	strum << "    <Addr>" << buf << "</Addr>" << endl;
    }

    strum << "    <Mac>";
    for (int i=0; i<x.Maclen; i++) {
	strum.fill('0');
	strum.width(2);
	strum << (hex) << (int) x.Mac[i];
	if (i<x.Maclen-1) strum  << ":";
    }
    strum << "</Mac>" << endl;

    SmartPtr<TIfaceSocket> sock;
    x.firstSocket();
    while (sock = x.getSocket() ) {
	strum << "    " << *sock;
    }
    strum << "    <!-- options -->" << endl;

    // option: DNS-SERVERS
    if (!x.DNSServerLstAddr || !x.DNSServerLstDUID) {
	strum << "    <!-- <dns-servers /> -->" << endl;
    } else {
	strum << "    <dns-servers addr=\"" << *x.DNSServerLstAddr << "\" duid=\"" 
	      << x.DNSServerLstDUID->getPlain() << "\" />" << endl;
    }
    x.DNSServerLst.first();
    while (addr = x.DNSServerLst.get()) {
	strum << "    <dns-server>" << *addr << "</dns-server>" << endl;
    }

    // option: DOMAINS
    if (!x.DomainLstAddr || !x.DomainLstDUID) {
	strum << "    <!-- <domains /> -->" << endl;
    } else {
	strum << "    <domains addr=\"" << *x.DomainLstAddr << "\" duid=\"" 
	      << x.DomainLstDUID->getPlain() << "\" />" << endl;
    }
    x.DomainLst.first();
    while (str = x.DomainLst.get()) {
	strum << "  <domain>" << *str << "</domain>" << endl;
    }

    strum << "  </ClntIfaceIface>" << endl;
    return strum;
}
