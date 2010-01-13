/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptDNSServers.cpp,v 1.13 2008-08-29 00:07:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.12  2007-01-21 19:17:57  thomson
 * Option name constants updated (by Jyrki Soini)
 *
 * Revision 1.11  2007-01-07 20:18:45  thomson
 * State enum names changed.
 *
 * Revision 1.10  2004-11-01 23:31:24  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.9  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.8  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.7  2004/10/03 21:28:45  thomson
 * 0.2.1-RC1 version.
 *
 * Revision 1.6  2004/05/23 20:41:03  thomson
 * *** empty log message ***
 *
 * Revision 1.5  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include "ClntMsg.h"
#include "ClntOptDNSServers.h"
#include "ClntOptServerIdentifier.h"
#include "ClntIfaceIface.h"
#include "Logger.h"

TClntOptDNSServers::TClntOptDNSServers(List(TIPv6Addr) * lst, TMsg* parent)
    :TOptAddrLst(OPTION_DNS_SERVERS, *lst, parent)
{
    
}

TClntOptDNSServers::TClntOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_DNS_SERVERS, buf,size, parent)
{

}

bool TClntOptDNSServers::doDuties()
{
    string reason = "trying to set DNS server(s).";
    int ifindex = this->Parent->getIface();
    SPtr<TIPv6Addr> addr = this->Parent->getAddr();
    TClntMsg * msg = (TClntMsg*)(this->Parent);
    SPtr<TClntIfaceMgr> ifaceMgr = msg->getClntIfaceMgr();
    if (!ifaceMgr) {
	Log(Error) << "Unable to access IfaceMgr while " << reason << LogEnd;
        return false;
    }
    SPtr<TClntIfaceIface> iface = (Ptr*)ifaceMgr->getIfaceByID(ifindex);
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << ifindex 
		   << " while " << reason << LogEnd;
        return false;
    }

    if (!this->DUID) {
	Log(Error) << "Unable to find proper DUID while " << reason << LogEnd;
	return false;
    }

    SPtr<TClntCfgMgr> cfgMgr = msg->getClntCfgMgr();
    SPtr<TClntCfgIface> cfgIface = cfgMgr->getIface(ifindex);
    cfgIface->setDNSServerState(STATE_CONFIGURED);

    return iface->setDNSServerLst(this->DUID, addr, this->AddrLst);
}

