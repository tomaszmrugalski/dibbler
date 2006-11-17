/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptAuthentication.h,v 1.1 2006-11-17 00:39:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 *
 */

#ifndef SRVOPTAUTHENTICATION_H
#define SRVOPTAUTHENTICATION_H

#include "OptAuthentication.h"

class TSrvOptAuthentication : public TOptAuthentication
{
 public:
    TSrvOptAuthentication(char * buf,  int n, TMsg* parent);
  
    TSrvOptAuthentication(TMsg* parent);
    bool doDuties();
};

#endif
