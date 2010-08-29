/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptSIPDomain.cpp,v 1.5 2008-08-29 00:07:29 thomson Exp $
 *
 */

#include "ClntOptSIPDomain.h"
#include "Portable.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptSIPDomain::TClntOptSIPDomain(List(string) *domains, TMsg* parent)    
    :TOptStringLst(OPTION_SIP_SERVER_D, *domains, parent) {
}

TClntOptSIPDomain::TClntOptSIPDomain(char *buf, int bufsize, TMsg* parent)
    :TOptStringLst(OPTION_SIP_SERVER_D, buf,bufsize, parent) {
}

bool TClntOptSIPDomain::doDuties() {
    string reason = "trying to set SIP domain(s).";
    int ifindex = this->Parent->getIface();

    if (!this->DUID) {
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
    cfgIface->setSIPDomainState(STATE_CONFIGURED);

    return iface->setSIPDomainLst(DUID, Parent->getAddr(), StringLst);
}

/// @todo remove
void TClntOptSIPDomain::setSrvDuid(SPtr<TDUID> duid) {
    this->SrvDUID=duid;
}
