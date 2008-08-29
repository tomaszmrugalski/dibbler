/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptLifetime.cpp,v 1.6 2008-08-29 00:07:29 thomson Exp $
 *
 */
#include "DHCPConst.h"
#include "ClntOptLifetime.h"
#include "ClntOptServerIdentifier.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptLifetime::TClntOptLifetime(char * buf,  int n, TMsg* parent)
	:TOptInteger(OPTION_INFORMATION_REFRESH_TIME, OPTION_INFORMATION_REFRESH_TIME_LEN, buf,n, parent){

}

TClntOptLifetime::TClntOptLifetime( char pref, TMsg* parent)
	:TOptInteger(OPTION_INFORMATION_REFRESH_TIME, OPTION_INFORMATION_REFRESH_TIME_LEN, pref, parent) {
}

bool TClntOptLifetime::doDuties()
{
    string reason = "trying to set Lifetime timer.";
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

    return iface->setLifetime(this->DUID, addr, this->Value);
}
