/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#ifndef CLNTVENDORSPECINFO_H_HEADER_INCLUDED_C1123F31
#define CLNTVENDORSPECINFO_H_HEADER_INCLUDED_C1123F31

#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

//##ModelId=3EC75CE3034A
class TClntOptVendorSpecInfo : public TOptVendorSpecInfo
{
  public:
    TClntOptVendorSpecInfo( char * buf,  int n, TMsg* parent);

};

#endif /* VENDORSPECINFO_H_HEADER_INCLUDED_C1123F31 */
