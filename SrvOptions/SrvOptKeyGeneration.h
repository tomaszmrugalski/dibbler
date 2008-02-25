/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptKeyGeneration.h,v 1.1 2008-02-25 20:42:46 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef SRVOPTKEYGENERATION_H
#define SRVOPTKEYGENERATION_H

#include "OptKeyGeneration.h"
#include "SrvMsg.h"

class TSrvOptKeyGeneration : public TOptKeyGeneration
{
 public:
    TSrvOptKeyGeneration(char * buf,  int n, TMsg* parent);
  
    TSrvOptKeyGeneration(TSrvMsg* parent);
    bool doDuties();
};

#endif
