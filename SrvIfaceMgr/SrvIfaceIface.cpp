/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceIface.cpp,v 1.2 2005-01-03 23:13:57 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/03 21:56:27  thomson
 * Initial version.
 *
 */

#include "SrvIfaceIface.h"

/*
  this one is used to create relay interfaces
**/
TSrvIfaceIface::TSrvIfaceIface(const char * name, int id, unsigned int flags, char* mac, 
			 int maclen, char* llAddr, int llAddrCnt, int hwType)
    :TIfaceIface(name,id, flags, mac, maclen, llAddr, llAddrCnt, hwType)
{
    this->Relay = false;
}

void TSrvIfaceIface::setUnderlaying(SmartPtr<TSrvIfaceIface> under) {
    this->UnderRelay = under;
    this->Relay = true;
}

SmartPtr<TSrvIfaceIface> TSrvIfaceIface::getUnderlaying() {
    return this->UnderRelay;
}

bool TSrvIfaceIface::appendRelay(SmartPtr<TSrvIfaceIface> relay, int interfaceID) {
    if (this->RelaysCnt>=MAX_RELAYS) 
	return false;
    this->Relays[this->RelaysCnt].iface       = relay;
    this->Relays[this->RelaysCnt].ifindex     = relay->getID();
    this->Relays[this->RelaysCnt].interfaceID = interfaceID;
    this->RelaysCnt++;
    return true;
}

SmartPtr<TSrvIfaceIface> TSrvIfaceIface::getRelayByInterfaceID(int interfaceID) {
    int i=0;
    for (i=0; i<this->RelaysCnt; i++) {
	if (this->Relays[i].interfaceID == interfaceID)
	    return this->Relays[i].iface;
    }
    return 0;
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
    strum << " flags=\"" << x.Flags << "\">" << endl;
    
    if (x.Relay) {
	strum << "    <UnderlayingRelay name=\"" << x.UnderRelay->getName() << "\" ifindex=\"" 
	      << x.UnderRelay->getID() << "\" />" << endl;
    }

    if (x.RelaysCnt) {
	for (i=0; i< x.RelaysCnt; i++) {
	    strum << "    <OverlayingRelay name=\"" << x.Relays[i].iface->getName() 
		  << "\" ifindex=\"" << x.Relays[i].ifindex 
		  << "\" interfaceID=\"" << x.Relays[i].interfaceID << "\" />" << endl;
	}
    }

    strum << "    <!-- " << x.LLAddrCnt << " link scoped addrs -->" << endl;

    for (int i=0; i<x.LLAddrCnt; i++) {
	inet_ntop6(x.LLAddr+i*16,buf);
	strum << "    <Addr>" << buf << "</Addr>" << endl;
    }

    strum << "    <Mac length=\"" << x.Maclen << "\">";
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
    strum << "  </IfaceIface>" << endl;
    return strum;
}
