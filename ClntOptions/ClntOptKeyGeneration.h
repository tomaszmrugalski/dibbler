/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptKeyGeneration.h,v 1.1 2008-02-25 20:42:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTKEYGENERATION_H_HEADER_INCLUDED
#define CLNTKEYGENERATION_H_HEADER_INCLUDED

#include "DHCPConst.h"
#include "OptKeyGeneration.h"
#include "ClntMsg.h"

class TClntOptKeyGeneration : public TOptKeyGeneration
{
  public:
    TClntOptKeyGeneration( char * buf,  int n, TClntMsg* parent);
	bool doDuties();
};


#endif /* KEYGENERATION_H_HEADER_INCLUDED */
