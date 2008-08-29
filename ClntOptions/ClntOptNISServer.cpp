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
#include "ClntOptServerIdentifier.h"
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
    cfgIface->setNISServerState(STATE_CONFIGURED);

    return iface->setNISServerLst(this->DUID, addr,this->AddrLst);
}

void TClntOptNISServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
