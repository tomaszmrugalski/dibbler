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
    
    if (!DUID) {
        Log(Error) << "Unable to find proper DUID while " << reason << LogEnd;
        return false;
    }

    SPtr<TClntIfaceIface> iface = (Ptr*)ClntIfaceMgr().getIfaceByID(ifindex);
    if (!iface) {
        Log(Error) << "Unable to find interface ifindex=" << ifindex
            << reason << LogEnd;
        return false;
    }

    SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface(ifindex);
    cfgIface->setNISPDomainState(STATE_CONFIGURED);

    return iface->setNISPDomain(DUID, Parent->getAddr(), Str);
}

/// @todo remove
void TClntOptNISPDomain::setSrvDuid(SPtr<TDUID> duid) {
    this->SrvDUID=duid;
}
