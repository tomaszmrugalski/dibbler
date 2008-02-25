/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptAuthentication.cpp,v 1.3 2008-02-25 17:49:08 thomson Exp $
 *
 */

#include "ClntOptAuthentication.h"
#include "ClntMsg.h"

TClntOptAuthentication::TClntOptAuthentication( char * buf,  int n, TMsg* parent)
	:TOptAuthentication(buf, n, parent)
{
}

TClntOptAuthentication::TClntOptAuthentication(TMsg* parent)
    :TOptAuthentication(parent)
{
    setRDM(0);
}

bool TClntOptAuthentication::doDuties()
{
    int ifindex = this->Parent->getIface();
    TClntMsg * msg = (TClntMsg*)(this->Parent);
    SmartPtr<TClntCfgMgr> cfgMgr = msg->getClntCfgMgr();
    SmartPtr<TClntCfgIface> cfgIface = cfgMgr->getIface(ifindex);
    cfgIface->setAuthenticationState(STATE_CONFIGURED);

    return false;
}
