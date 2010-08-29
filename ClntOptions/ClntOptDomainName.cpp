/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntOptDomainName.h"
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
    int ifindex = Parent->getIface();

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
    cfgIface->setDomainState(STATE_CONFIGURED);

    return iface->setDomainLst(DUID, Parent->getAddr(), StringLst);
}

void TClntOptDomainName::setSrvDuid(SPtr<TDUID> duid) {
    SrvDUID=duid;
}
