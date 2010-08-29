/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntOptNTPServers.h"
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
    cfgIface->setNTPServerState(STATE_CONFIGURED);

    return iface->setNTPServerLst(DUID, Parent->getAddr(), AddrLst);
}

#if 0
/// @todo Remove this if nobody uses it
void TClntOptNTPServers::setSrvDuid(SPtr<TDUID> duid)
{
    SrvDUID=duid;
}
#endif
