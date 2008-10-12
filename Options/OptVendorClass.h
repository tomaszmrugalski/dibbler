/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

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

    int getVendor();
    char * getVendorData();      // returns vendor data (binary)
    string getVendorDataPlain(); // returns vendor data (as a printable string)
    int getVendorDataLen();      // returns vendor data length

  protected:
      int Vendor;
      char * VendorData;
      int VendorDataLen;

};



#endif /* VENDORCLASS_H_HEADER_INCLUDED_C1125E8B */
