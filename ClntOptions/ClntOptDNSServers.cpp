/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntMsg.h"
#include "ClntOptDNSServers.h"
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
    int ifindex = Parent->getIface();

    if (!DUID) {
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
    cfgIface->setDNSServerState(STATE_CONFIGURED);

    return iface->setDNSServerLst(DUID, Parent->getAddr(), AddrLst);
}

