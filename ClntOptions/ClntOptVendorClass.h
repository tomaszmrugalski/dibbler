/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTVENDORCLASS_H
#define CLNTVENDORCLASS_H

#include "OptVendorData.h"
#include "DHCPConst.h"

class TClntOptVendorClass : public TOptVendorData
{
  public:
    TClntOptVendorClass( char * buf,  int n, TMsg* parent);
	bool doDuties();
};

#endif

