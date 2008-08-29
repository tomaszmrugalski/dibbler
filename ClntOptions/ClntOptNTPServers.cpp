/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptNTPServers.cpp,v 1.9 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2007-01-21 19:17:57  thomson
 * Option name constants updated (by Jyrki Soini)
 *
 * Revision 1.7  2007-01-07 20:18:46  thomson
 * State enum names changed.
 *
 * Revision 1.6  2004-11-01 23:31:25  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.5  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.4  2004/03/29 19:10:06  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include "ClntOptNTPServers.h"
#include "ClntOptServerIdentifier.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptNTPServers::TClntOptNTPServers(List(TIPv6Addr) * lst, TMsg* parent)
    :TOptAddrLst(OPTION_SNTP_SERVERS, *lst, parent) {
}

TClntOptNTPServers::TClntOptNTPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_SNTP_SERVERS, buf, size, parent) {
}

bool TClntOptNTPServers::doDuties() {
    string reason = "trying to set NTP server(s).";
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
    cfgIface->setNTPServerState(STATE_CONFIGURED);

    return iface->setNTPServerLst(this->DUID, addr,this->AddrLst);
}

void TClntOptNTPServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
