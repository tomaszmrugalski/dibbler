/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTVENDORCLASS_H_HEADER_INCLUDED_C1125E8B
#define CLNTVENDORCLASS_H_HEADER_INCLUDED_C1125E8B

#include "OptVendorClass.h"
#include "DHCPConst.h"

//##ModelId=3EC75CE30346
class TClntOptVendorClass : public TOptVendorClass 
{
  public:
    TClntOptVendorClass( char * buf,  int n, TMsg* parent);
	bool doDuties();
};

#endif /* VENDORCLASS_H_HEADER_INCLUDED_C1125E8B */
