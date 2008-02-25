/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptAAAAuthentication.cpp,v 1.1 2008-02-25 20:42:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "ClntOptAAAAuthentication.h"

TClntOptAAAAuthentication::TClntOptAAAAuthentication( char * buf,  int n, TMsg* parent)
	:TOptAAAAuthentication(buf, n, parent)
{
}

TClntOptAAAAuthentication::TClntOptAAAAuthentication(TClntMsg* parent)
    :TOptAAAAuthentication(parent)
{
    this->setAAASPI(parent->getClntCfgMgr()->getAAASPI());
}

bool TClntOptAAAAuthentication::doDuties()
{
    return false;
}
