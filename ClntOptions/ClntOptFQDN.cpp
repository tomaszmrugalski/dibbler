/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptFQDN.cpp,v 1.3 2006-03-02 00:59:22 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/11/01 23:31:25  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.1  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
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
		Log(Notice) << "DHCP server made the DNS update with the FQDN" << getFQDN() << LogEnd;
		//TODO Check the DNS server with the given name.
		return true;
	}
	
	string reason = "trying to set FQDN.";
	int ifindex = this->Parent->getIface();
	Log(Debug) << "FQDN option : IfaceID = " << ifindex << LogEnd;
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
	
	return iface->setFQDN(this->DUID, addr,getFQDN());
}

void TClntOptFQDN::setSrvDuid(SmartPtr<TDUID> duid)

{
    this->SrvDUID=duid;
}


