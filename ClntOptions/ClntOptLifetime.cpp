/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptLifetime.cpp,v 1.3 2006-10-29 13:11:46 thomson Exp $
 *
 */
#include "DHCPConst.h"
#include "ClntOptLifetime.h"
#include "ClntOptServerIdentifier.h"
#include "ClntMsg.h"

TClntOptLifetime::TClntOptLifetime(char * buf,  int n, TMsg* parent)
	:TOptInteger(OPTION_LIFETIME, OPTION_LIFETIME_LEN, buf,n, parent){

}

TClntOptLifetime::TClntOptLifetime( char pref, TMsg* parent)
	:TOptInteger(OPTION_LIFETIME, OPTION_LIFETIME_LEN, pref, parent) {
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
