/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptAAAAuthentication.h,v 1.1 2008-02-25 20:42:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTAAAAUTHENTICATION_H_HEADER_INCLUDED
#define CLNTAAAAUTHENTICATION_H_HEADER_INCLUDED

#include "DHCPConst.h"
#include "OptAAAAuthentication.h"
#include "ClntMsg.h"

class TClntOptAAAAuthentication : public TOptAAAAuthentication
{
  public:
    TClntOptAAAAuthentication( char * buf,  int n, TMsg* parent);
    TClntOptAAAAuthentication(TClntMsg* parent);
	bool doDuties();
};


#endif /* AAAAUTHENTICATION_H_HEADER_INCLUDED */
