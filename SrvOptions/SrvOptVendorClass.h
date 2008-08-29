/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef SRVVENDORCLASS_H_HEADER_INCLUDED_C1125E8B
#define SRVVENDORCLASS_H_HEADER_INCLUDED_C1125E8B

#include "OptVendorClass.h"
#include "DHCPConst.h"

class TSrvOptVendorClass : public TOptVendorClass 
{
  public:
    TSrvOptVendorClass( char * buf,  int n, TMsg* parent);
    bool doDuties();
};

#endif
