#ifndef VENDORCLASS_H_HEADER_INCLUDED_C1125E8B
#define VENDORCLASS_H_HEADER_INCLUDED_C1125E8B

#include "Opt.h"
#include "DHCPConst.h"

class TOptVendorClass : public TOpt
{
  public:
    TOptVendorClass( char * &buf,  int &n, TMsg* parent);
    int getSize();
    char * storeSelf( char* buf);
    bool isValid();
};



#endif /* VENDORCLASS_H_HEADER_INCLUDED_C1125E8B */
