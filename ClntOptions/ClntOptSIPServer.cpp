/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptSIPServer.cpp,v 1.1 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    SmartPtr<TClntOptServerIdentifier> optSrvID = (Ptr*)msg->getOption(OPTION_SERVERID);
    if (!optSrvID) {
	Log(Error) << "Unable to find ServerID option while " << reason << LogEnd;
	return false;
    }
    SmartPtr<TDUID> duid = optSrvID->getDUID();

    return iface->setSIPServerLst(duid, addr,this->AddrLst);
}

void TClntOptSIPServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
