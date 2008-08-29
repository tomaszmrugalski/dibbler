/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptDomainName.cpp,v 1.11 2008-08-29 00:07:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2007-01-07 20:18:45  thomson
 * State enum names changed.
 *
 * Revision 1.9  2004-11-01 23:31:24  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.8  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.7  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.6  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.5  2004/03/29 19:10:06  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include "ClntOptDomainName.h"
#include "ClntOptServerIdentifier.h"
#include "Portable.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptDomainName::TClntOptDomainName(List(string) * domains, TMsg* parent)    
    :TOptStringLst(OPTION_DOMAIN_LIST, *domains, parent) {
}

TClntOptDomainName::TClntOptDomainName(char *buf, int bufsize, TMsg* parent)
    :TOptStringLst(OPTION_DOMAIN_LIST, buf,bufsize, parent) {
}

bool TClntOptDomainName::doDuties() {
    string reason = "trying to set domain(s).";
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
    cfgIface->setDomainState(STATE_CONFIGURED);

    return iface->setDomainLst(this->DUID, addr, this->StringLst);
}

void TClntOptDomainName::setSrvDuid(SmartPtr<TDUID> duid) {
    this->SrvDUID=duid;
}
