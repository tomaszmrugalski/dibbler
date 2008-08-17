/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptFQDN.cpp,v 1.8 2008-08-17 22:41:42 thomson Exp $
 *
 */

#include "ClntOptFQDN.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptIAAddress.h"
#include "ClntOptIA_NA.h"
#include "AddrIA.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptFQDN::TClntOptFQDN(string domain, TMsg* parent)
    :TOptFQDN(domain, parent) {

	if (domain == "") {
		// The domain is empty but client request FQDN, so we let the server make the update
		setSFlag(true);
	}
}

TClntOptFQDN::TClntOptFQDN(char *buf, int bufsize, TMsg* parent)
    :TOptFQDN(buf, bufsize, parent) {
    // FIXME: do some validity check

}


bool TClntOptFQDN::doDuties() {
    if (getSFlag()) {
	Log(Notice) << "FQDN: DHCPv6 server made the DNS update for my name: " << getFQDN() << " ." << LogEnd;
	//FIXME: Check the DNS server with the given name.
	return true;
    }
	
    string reason = "trying to set FQDN.";
    int ifindex = this->Parent->getIface();
    SmartPtr<TIPv6Addr> addr = this->Parent->getAddr();
    TClntMsg * msg = (TClntMsg*)(this->Parent);
    SmartPtr<TClntIfaceMgr> ifaceMgr = msg->getClntIfaceMgr();
    
    if (!ifaceMgr) {
	Log(Error) << "Unable to access IfaceMgr while " << reason << LogEnd;
	return false;
    }
    
    SmartPtr<TClntIfaceIface> iface = (Ptr*)ifaceMgr->getIfaceByID(ifindex);
    
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << ifindex 
		   << " while " << reason << LogEnd;
	return false;
    }
    
    if (!this->DUID) {
	Log(Error) << "Unable to find proper DUID while " << reason << LogEnd;
	return false;
    }
    
    return iface->setFQDN(this->DUID, addr,getFQDN()); // this runs only when client is gonna update DNS server 
}

void TClntOptFQDN::setSrvDuid(SmartPtr<TDUID> duid)

{
    this->SrvDUID=duid;
}


