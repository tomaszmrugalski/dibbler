/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptDNSServers.cpp,v 1.8 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    :TOptAddrLst(OPTION_DNS_RESOLVERS, *lst, parent)
{
    
}

TClntOptDNSServers::TClntOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_DNS_RESOLVERS, buf,size, parent)
{

}

bool TClntOptDNSServers::doDuties()
{
    string reason = "trying to set DNS server(s).";
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

    return iface->setDNSServerLst(duid, addr,this->AddrLst);
}

void TClntOptDNSServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
