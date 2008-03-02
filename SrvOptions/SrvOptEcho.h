/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptEcho.h,v 1.1 2008-03-02 19:54:07 thomson Exp $
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
