/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptFQDN.cpp,v 1.2 2004-11-01 23:31:25 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#include "ClntOptFQDN.h"
#include "ClntOptServerIdentifier.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptFQDN::TClntOptFQDN(string domain, TMsg* parent)
    :TOptString(OPTION_FQDN, domain, parent) {

}

TClntOptFQDN::TClntOptFQDN(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_FQDN, buf,bufsize, parent) {
    // FIXME: do some validity check

}

bool TClntOptFQDN::doDuties() {
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

    SmartPtr<TClntCfgMgr> cfgMgr = msg->getClntCfgMgr();
    SmartPtr<TClntCfgIface> cfgIface = cfgMgr->getIface(ifindex);
    cfgIface->setFQDNState(CONFIGURED);

    return iface->setFQDN(this->DUID, addr,this->Str);
}

void TClntOptFQDN::setSrvDuid(SmartPtr<TDUID> duid)

{
    this->SrvDUID=duid;
}

