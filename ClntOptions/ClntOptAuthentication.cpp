/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptAuthentication.cpp,v 1.2 2006-11-24 01:33:43 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2006-11-17 00:38:54  thomson
 * Partial AUTH support by Sammael, fixes by thomson
 *
 *
 */
#include "ClntOptAuthentication.h"

TClntOptAuthentication::TClntOptAuthentication( char * buf,  int n, TMsg* parent)
	:TOptAuthentication(buf, n, parent)
{
}

TClntOptAuthentication::TClntOptAuthentication(uint64_t ReplayDet, TMsg* parent)
    :TOptAuthentication(parent)
{
    setRDM(0);
    setReplayDet(ReplayDet);
    setSPI(69);
}

bool TClntOptAuthentication::doDuties()
{
    return false;
}
