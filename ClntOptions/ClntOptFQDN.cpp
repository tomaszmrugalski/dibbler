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
    /// @todo: do some validity check

}


bool TClntOptFQDN::doDuties() {
    if (getSFlag()) {
	    Log(Notice) << "FQDN: DHCPv6 server made the DNS update for my name: " << getFQDN() << " ." << LogEnd;
	    /// @todo: Check the DNS server with the given name.
	    return true;
    }
	
    string reason = "trying to set FQDN.";
    int ifindex = this->Parent->getIface();
    SPtr<TIPv6Addr> addr = this->Parent->getAddr();
    TClntMsg * msg = (TClntMsg*)(this->Parent);
    
    SPtr<TClntIfaceIface> iface = (Ptr*)ClntIfaceMgr().getIfaceByID(ifindex);
    
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

void TClntOptFQDN::setSrvDuid(SPtr<TDUID> duid)

{
    this->SrvDUID=duid;
}


