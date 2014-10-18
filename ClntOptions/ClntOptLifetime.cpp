/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */
#include "DHCPConst.h"
#include "ClntOptLifetime.h"
#include "OptDUID.h"
#include "ClntMsg.h"
#include "Logger.h"

using namespace std;

TClntOptLifetime::TClntOptLifetime(char * buf,  int n, TMsg* parent)
	:TOptInteger(OPTION_INFORMATION_REFRESH_TIME, OPTION_INFORMATION_REFRESH_TIME_LEN, buf,n, parent){

}

TClntOptLifetime::TClntOptLifetime( char pref, TMsg* parent)
	:TOptInteger(OPTION_INFORMATION_REFRESH_TIME, OPTION_INFORMATION_REFRESH_TIME_LEN, pref, parent) {
}

bool TClntOptLifetime::doDuties()
{
    string reason = "trying to set Lifetime timer.";

    if (!Parent) {
        Log(Error) << "Unable to set lifetime: option parent not set." << LogEnd;
        return false;
    }

    int ifindex = Parent->getIface();

    SPtr<TOptDUID> duid = (Ptr*)Parent->getOption(OPTION_SERVERID);

    if (!duid) {
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
    return iface->setLifetime(duid->getDUID(), Parent->getRemoteAddr(), Value);
}
