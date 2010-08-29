/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptSIPServer.cpp,v 1.5 2008-08-29 00:07:29 thomson Exp $
 */

#include "ClntOptSIPServer.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptSIPServers::TClntOptSIPServers(List(TIPv6Addr) *lst, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVER_A, *lst, parent) {
}

TClntOptSIPServers::TClntOptSIPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVER_A, buf, size, parent) {
}

bool TClntOptSIPServers::doDuties() {
    string reason = "trying to set SIP server(s).";
    int ifindex = Parent->getIface();

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
    cfgIface->setSIPServerState(STATE_CONFIGURED);

    return iface->setSIPServerLst(DUID, Parent->getAddr(), AddrLst);
}

/// @todo remove
void TClntOptSIPServers::setSrvDuid(SPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
