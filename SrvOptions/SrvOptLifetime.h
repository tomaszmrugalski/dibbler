/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptLifetime.h,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef SRVOPTLIFETIME_H
#define SRVOPTLIFETIME_H

#include "DHCPConst.h"
#include "OptInteger4.h"

class TSrvOptLifetime : public TOptInteger4
{
 public:
    TSrvOptLifetime(char * buf,  int n, TMsg* parent);
    
    TSrvOptLifetime(unsigned int lifetime, TMsg* parent);
    bool doDuties();
};

#endif
