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
