/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceIface.cpp,v 1.14 2008-11-11 22:41:49 thomson Exp $
 *
 */

#include "Logger.h"
#include "SrvIfaceIface.h"

/*
  this one is used to create relay interfaces
**/
TSrvIfaceIface::TSrvIfaceIface(const char * name, int id, unsigned int flags, char* mac, 
			       int maclen, char* llAddr, int llAddrCnt, char * globalAddr, int globalAddrCnt,
			       int hwType)
    :TIfaceIface(name,id, flags, mac, maclen, llAddr, llAddrCnt, globalAddr, globalAddrCnt, hwType)
{
    this->Relay = false;
    this->RelaysCnt = 0;
}

void TSrvIfaceIface::setUnderlaying(SPtr<TSrvIfaceIface> under) {
    this->UnderRelay = under;
    this->Relay = true;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getUnderlaying() {
    return this->UnderRelay;
}

bool TSrvIfaceIface::appendRelay(SPtr<TSrvIfaceIface> relay, SPtr<TSrvOptInterfaceID> interfaceID) {
    if (this->RelaysCnt>=HOP_COUNT_LIMIT) 
	return false;
    this->Relays[this->RelaysCnt].iface       = relay;
    this->Relays[this->RelaysCnt].ifindex     = relay->getID();
    this->Relays[this->RelaysCnt].interfaceID = interfaceID;
    this->RelaysCnt++;
    return true;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getRelayByInterfaceID(SPtr<TSrvOptInterfaceID> interfaceID) {
    int i=0;
    if (this->RelaysCnt==0) {
	Log(Warning) << "No relay interface defined on the " << this->getFullName() << LogEnd;
	return 0;
    }
    for (i=0; i<this->RelaysCnt; i++) {
	if (*Relays[i].interfaceID == *interfaceID)
	    return this->Relays[i].iface;
    }
    return 0;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getRelayByLinkAddr(SPtr<TIPv6Addr> addr) {
    if (this->RelaysCnt==0) {
	Log(Warning) << "No relay interface defined on the " << this->getFullName() << LogEnd;
	return 0;
    }
    //// @todo: Implement finding RELAYs using link address
    Log(Error) << "Finding RELAYs using link address is not implemented yet. Using first relay:" 
	      << this->Relays[0].iface->getFullName() << LogEnd;
    return this->Relays[0].iface;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getAnyRelay() {
    if (this->RelaysCnt==0) {
	Log(Warning) << "No relay interface defined on the " << this->getFullName() << LogEnd;
	return 0;
    }
    return this->Relays[0].iface;
}

int TSrvIfaceIface::getRelayCnt() {
    return this->RelaysCnt;
}


// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * just prints important informations (debugging & logging)
 */
ostream & operator <<(ostream & strum, TSrvIfaceIface &x) {
    char buf[48];
    int i;

    strum << dec;
    strum << "  <SrvIfaceIface";
    strum << " name=\"" << x.Name << "\"";
    strum << " ifindex=\"" << x.ID << "\"";
    strum << " hwType=\"" << x.getHardwareType() << "\"";
    strum << " flags=\"" << x.Flags << "\">" << endl;
    
    if (x.Relay) {
	strum << "    <UnderlayingRelay name=\"" << x.UnderRelay->getName() << "\" ifindex=\"" 
	      << x.UnderRelay->getID() << "\" />" << endl;
    }

    if (x.RelaysCnt) {
	for (i=0; i< x.RelaysCnt; i++) {
	    strum << "    <OverlayingRelay name=\"" << x.Relays[i].iface->getName() 
		  << "\" ifindex=\"" << x.Relays[i].ifindex;
	    if (x.Relays[i].interfaceID)
	      strum << "\" interfaceID=\"" << x.Relays[i].interfaceID->getPlain();
	    else
	      strum << "\" interfaceID=null\"";
	    strum << "\" />" << endl;
	}
    }

    strum << "    <!-- " << x.LLAddrCnt << " link scoped addrs -->" << endl;

    for (int i=0; i<x.LLAddrCnt; i++) {
	inet_ntop6(x.LLAddr+i*16,buf);
	strum << "    <Addr scope=\"local\">" << buf << "</Addr>" << endl;
    }

    strum << "    <!-- " << x.countGlobalAddr() << " non-local (global) addrs -->" << endl;

    x.firstGlobalAddr();
    SPtr<TIPv6Addr> addr;
    while (addr = x.getGlobalAddr()) {
      	strum << "    <Addr scope=\"global\">" << *addr << "</Addr>" << endl;
    }

    strum << "    <Mac length=\"" << x.Maclen << "\">";
    for (int i=0; i<x.Maclen; i++) {
	strum.fill('0');
	strum.width(2);
	strum << (hex) << (int) x.Mac[i];
	if (i<x.Maclen-1) strum  << ":";
    }
    strum << "</Mac>" << endl;

    strum << "    <!-- " << x.countSocket() << " sockets -->" << endl;
    SPtr<TIfaceSocket> sock;
    x.firstSocket();
    while (sock = x.getSocket() ) {
	strum << "    " << *sock;
    }
    strum << "  </SrvIfaceIface>" << endl;
    return strum;
}
