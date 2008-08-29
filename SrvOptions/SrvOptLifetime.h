/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptLifetime.h,v 1.3 2008-08-29 00:07:36 thomson Exp $
 *
 */

#ifndef SRVOPTLIFETIME_H
#define SRVOPTLIFETIME_H

#include "DHCPConst.h"
#include "OptInteger.h"

class TSrvOptLifetime : public TOptInteger
{
 public:
    TSrvOptLifetime(char * buf,  int n, TMsg* parent);
    TSrvOptLifetime(unsigned int lifetime, TMsg* parent);
    bool doDuties();
};

#endif
