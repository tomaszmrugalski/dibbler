/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptKeyGeneration.h,v 1.2 2008-08-29 00:07:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2008-02-25 20:42:46  thomson
 * Missing new AUTH files added.
 *
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
