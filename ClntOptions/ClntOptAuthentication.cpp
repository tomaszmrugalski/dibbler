/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptAuthentication.cpp,v 1.1 2006-11-17 00:38:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "ClntOptAuthentication.h"

TClntOptAuthentication::TClntOptAuthentication( char * buf,  int n, TMsg* parent)
	:TOptAuthentication(buf, n, parent)
{
}

TClntOptAuthentication::TClntOptAuthentication(TMsg* parent)
    :TOptAuthentication(parent)
{
    setRDM(42);
    setReplayDet(23);
    setSPI(69);
}

bool TClntOptAuthentication::doDuties()
{
    return false;
}
