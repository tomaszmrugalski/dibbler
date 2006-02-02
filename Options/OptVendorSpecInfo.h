/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#ifndef VENDORSPECINFO_H
#define VENDORSPECINFO_H

#include "Opt.h"
#include "DHCPConst.h"

class TOptVendorSpecInfo : public TOpt
{
  public:
    TOptVendorSpecInfo( char * &buf,  int &n,TMsg* parent);
    int getSize();
    char * storeSelf( char* buf);
    bool isValid();
};

#endif
