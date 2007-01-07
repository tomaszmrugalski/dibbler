/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptSIPServer.cpp,v 1.3 2007-01-07 20:18:46 thomson Exp $
 */

#include "ClntOptSIPServer.h"
#include "ClntOptServerIdentifier.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptSIPServers::TClntOptSIPServers(List(TIPv6Addr) *lst, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVERS, *lst, parent) {
}

TClntOptSIPServers::TClntOptSIPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVERS, buf, size, parent) {
}

bool TClntOptSIPServers::doDuties() {
    string reason = "trying to set SIP server(s).";
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
    cfgIface->setSIPServerState(STATE_CONFIGURED);

    return iface->setSIPServerLst(this->DUID, addr,this->AddrLst);
}

void TClntOptSIPServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
