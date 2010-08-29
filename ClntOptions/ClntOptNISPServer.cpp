/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptNISPServer.cpp,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 */

#include "ClntOptNISPServer.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptNISPServers::TClntOptNISPServers(List(TIPv6Addr) * lst, TMsg* parent)
    :TOptAddrLst(OPTION_NISP_SERVERS, *lst, parent) {
}

TClntOptNISPServers::TClntOptNISPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_NISP_SERVERS, buf, size, parent) {
}

bool TClntOptNISPServers::doDuties() {
    string reason = "trying to set NIS+ server(s).";
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
    cfgIface->setNISPServerState(STATE_CONFIGURED);

    return iface->setNISPServerLst(DUID, Parent->getAddr(), AddrLst);
}

/// @todo remove
void TClntOptNISPServers::setSrvDuid(SPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
