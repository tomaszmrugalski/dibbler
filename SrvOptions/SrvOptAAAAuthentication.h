/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptAAAAuthentication.h,v 1.1 2008-02-25 20:42:46 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef SRVOPTAAAAUTHENTICATION_H
#define SRVOPTAAAAUTHENTICATION_H

#include "OptAAAAuthentication.h"

class TSrvOptAAAAuthentication : public TOptAAAAuthentication
{
 public:
    TSrvOptAAAAuthentication(char * buf,  int n, TMsg* parent);
  
    TSrvOptAAAAuthentication(TMsg* parent);
    bool doDuties();
};

#endif
