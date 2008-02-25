/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptKeyGeneration.cpp,v 1.1 2008-02-25 20:42:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "ClntOptKeyGeneration.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptKeyGeneration::TClntOptKeyGeneration( char * buf,  int n, TClntMsg* parent)
	:TOptKeyGeneration(buf, n, parent)
{
    parent->getClntCfgMgr()->setDigest((DigestTypes)this->getAlgorithmId());
}

bool TClntOptKeyGeneration::doDuties()
{
    int ifindex = this->Parent->getIface();
    TClntMsg * msg = (TClntMsg*)(this->Parent);
    SmartPtr<TClntCfgMgr> cfgMgr = msg->getClntCfgMgr();
    SmartPtr<TClntCfgIface> cfgIface = cfgMgr->getIface(ifindex);
    cfgIface->setKeyGenerationState(STATE_CONFIGURED);

    return true;
}
