#ifndef SRVVENDORSPECINFO_H_HEADER_INCLUDED_C1123F31
#define SRVVENDORSPECINFO_H_HEADER_INCLUDED_C1123F31

#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

class TSrvOptVendorSpecInfo : public TOptVendorSpecInfo
{
 public:
    TSrvOptVendorSpecInfo( char * buf,  int n, TMsg* parent);
    bool doDuties();
};

#endif /* VENDORSPECINFO_H_HEADER_INCLUDED_C1123F31 */
