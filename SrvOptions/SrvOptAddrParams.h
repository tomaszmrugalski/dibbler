/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptAddrParams.h,v 1.1.2.2 2007-04-15 21:23:33 thomson Exp $
 *
 */

#ifndef SRVOPTADDRPARAMS_H
#define SRVOPTADDRPARAMS_H
#include "OptInteger.h"

class TSrvOptAddrParams : public TOptInteger
{
  public:
    TSrvOptAddrParams(int prefix, int bitfield, TMsg * parent);
    TSrvOptAddrParams(char * buf,  int n, TMsg* parent);

    int getPrefix();
    int getBitfield();
    bool doDuties();
};



#endif 
