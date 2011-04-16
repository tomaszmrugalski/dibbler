/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTVENDORSPECINFO_H
#define OPTVENDORSPECINFO_H

#include "Opt.h"
#include "DHCPConst.h"

class TOptVendorSpecInfo : public TOpt
{
  public:
    TOptVendorSpecInfo(int type, char * buf,  int n, TMsg* parent);
    TOptVendorSpecInfo(int type, int enterprise, char *data, int dataLen, TMsg* parent);

    int getSize();
    char * storeSelf( char* buf);
    bool isValid();

    unsigned int getVendor();
    ~TOptVendorSpecInfo();
    bool doDuties() { return true; }
protected:
    unsigned int Vendor;
};

#endif /* OPTVENDORSPECINFO_H */
