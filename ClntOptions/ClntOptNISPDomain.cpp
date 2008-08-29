/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptNISPDomain.cpp,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 */

#include "ClntOptNISPDomain.h"
#include "ClntOptServerIdentifier.h"
#include "Portable.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptNISPDomain::TClntOptNISPDomain(string domain, TMsg* parent)    
    :TOptString(OPTION_NISP_DOMAIN_NAME, domain, parent) {
}

TClntOptNISPDomain::TClntOptNISPDomain(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_NISP_DOMAIN_NAME, buf,bufsize, parent) {
}

bool TClntOptNISPDomain::doDuties() {
    string reason = "trying to set NIS+ domain.";
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
    cfgIface->setNISPDomainState(STATE_CONFIGURED);

    return iface->setNISPDomain(this->DUID, addr,this->Str);
}

void TClntOptNISPDomain::setSrvDuid(SmartPtr<TDUID> duid) {
    this->SrvDUID=duid;
}
