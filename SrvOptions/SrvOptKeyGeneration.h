/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVOPTKEYGENERATION_H
#define SRVOPTKEYGENERATION_H

#ifndef MOD_DISABLE_AUTH

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

#endif
