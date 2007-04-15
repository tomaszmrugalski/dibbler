/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptAddrParams.h,v 1.1.2.1 2007-04-15 19:26:33 thomson Exp $
 *
 */

#define ADDRPARAMS_MASK_PREFIX    0x01
#define ADDRPARAMS_MASK_ANYCAST   0x02
#define ADDRPARAMS_MASK_MULTICAST 0x04

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
