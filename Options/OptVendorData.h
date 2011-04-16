/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef VENDORCLASS_H
#define VENDORCLASS_H

#include "Opt.h"
//#include "DHCPConst.h"

class TOptVendorData : public TOpt
{
  public:
    TOptVendorData(int type, int enterprise, char * data, int dataLen, TMsg* parent);
    TOptVendorData(int type, char * buf,  int n, TMsg* parent);
    int getSize();
    char * storeSelf( char* buf);
    bool isValid();

    int getVendor();
    char * getVendorData();      // returns vendor data (binary)
    string getVendorDataPlain(); // returns vendor data (as a printable string)
    int getVendorDataLen();      // returns vendor data length
    bool doDuties() { return true; }

  protected:
    int Vendor;
    char * VendorData;
    int VendorDataLen;
};
#endif
