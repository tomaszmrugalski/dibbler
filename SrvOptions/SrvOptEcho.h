/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptEcho.h,v 1.2 2008-08-29 00:07:36 thomson Exp $
 *
 */

#ifndef SRVOPTECHO_H
#define SRVOPTECHO_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "OptOptionRequest.h"

class TSrvOptEcho;

class TSrvOptEcho : public TOptOptionRequest 
{
  public:
    TSrvOptEcho(char * buf,  int n, TMsg* parent);
    TSrvOptEcho(TMsg* parent);
    bool doDuties();
};

#endif
