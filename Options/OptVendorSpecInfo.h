#ifndef VENDORSPECINFO_H_HEADER_INCLUDED_C1123F31
#define VENDORSPECINFO_H_HEADER_INCLUDED_C1123F31

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
