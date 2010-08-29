/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptNISServer.cpp,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 */

#include "ClntOptNISServer.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptNISServers::TClntOptNISServers(List(TIPv6Addr) *lst, TMsg* parent)
    :TOptAddrLst(OPTION_NIS_SERVERS, *lst, parent) {
}

TClntOptNISServers::TClntOptNISServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_NIS_SERVERS, buf, size, parent) {
}

bool TClntOptNISServers::doDuties() {
    string reason = "trying to set NIS server(s).";
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
    cfgIface->setNISServerState(STATE_CONFIGURED);

    return iface->setNISServerLst(DUID, Parent->getAddr(), AddrLst);
}

/// @todo remove
void TClntOptNISServers::setSrvDuid(SPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
