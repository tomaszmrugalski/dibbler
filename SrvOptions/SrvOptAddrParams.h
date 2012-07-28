/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptAddrParams.h,v 1.3 2008-08-29 00:07:35 thomson Exp $
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
