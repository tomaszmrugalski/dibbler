/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptDNSServers.cpp,v 1.7 2004-10-03 21:28:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
#include "Logger.h"

TClntOptDNSServers::TClntOptDNSServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent)
    :TOptDNSServers(lst, parent)
{

}

TClntOptDNSServers::TClntOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptDNSServers(buf,size, parent)
{

}

bool TClntOptDNSServers::doDuties()
{
    int ifaceid = this->Parent->getIface();
    TClntMsg * msg = (TClntMsg*)(this->Parent);
	SmartPtr<TClntIfaceMgr> ifaceMgr = msg->getClntIfaceMgr();
	if (!ifaceMgr) {
		Log(Error) << "Unable to access IfaceMgr while setting up DNS server." << LogEnd;
        return false;
	}
	SmartPtr<TIfaceIface> iface = ifaceMgr->getIfaceByID(ifaceid);
	if (!iface) {
		Log(Error) << "Unable to find interface with ifindex=" << ifaceid << " while trying to set DNS." 
                   << LogEnd;
        return false;
	}

	SmartPtr<TIPv6Addr> ptrAddr;
    DNSSrv.first();

    while (ptrAddr = DNSSrv.get() ) {
		dns_add(iface->getName(),ifaceid,ptrAddr->getPlain() );
    }
	return true;
}

void TClntOptDNSServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
