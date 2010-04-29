/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "ClntOptFQDN.h"
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
	Log(Notice) << "FQDN: DHCPv6 server made the DNS update for my name: " 
		    << getFQDN() << " ." << LogEnd;
	/// @todo: Check the DNS server with the given name.
	return true;
    }
	
    string reason = "trying to set FQDN.";
    int ifindex = this->Parent->getIface();
    SPtr<TIPv6Addr> addr = this->Parent->getAddr();
    
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
    
    // this runs only when client is gonna update DNS server 
    return iface->setFQDN(this->DUID, addr,getFQDN()); 
}

void TClntOptFQDN::setSrvDuid(SPtr<TDUID> duid) {
    SrvDUID=duid;
}


