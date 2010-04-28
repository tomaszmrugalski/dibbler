/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptKeyGeneration.cpp,v 1.2 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2008-02-25 20:42:45  thomson
 * Missing new AUTH files added.
 *
 *
 */
#include "ClntOptKeyGeneration.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptKeyGeneration::TClntOptKeyGeneration( char * buf,  int n, TClntMsg* parent)
	:TOptKeyGeneration(buf, n, parent)
{
    ClntCfgMgr().setDigest((DigestTypes)this->getAlgorithmId());
}

bool TClntOptKeyGeneration::doDuties()
{
    int ifindex = Parent->getIface();
    SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface(ifindex);
    cfgIface->setKeyGenerationState(STATE_CONFIGURED);

    return true;
}
