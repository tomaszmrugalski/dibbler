/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTVENDORDATA_H
#define OPTVENDORDATA_H

#include "Opt.h"
//#include "DHCPConst.h"

class TOptVendorData : public TOpt
{
  public:
    TOptVendorData(int type, int enterprise, char * data, int dataLen, TMsg* parent);
    TOptVendorData(int type, const char * buf,  int n, TMsg* parent);
    size_t getSize();
    char * storeSelf( char* buf);
    virtual bool isValid() const;

    /// @todo: should return uint32_t
    int getVendor();
    char * getVendorData();      // returns vendor data (binary)
    std::string getVendorDataPlain(); // returns vendor data (as a printable string)
    int getVendorDataLen();      // returns vendor data length
    bool doDuties() { return true; }

  protected:
    int Vendor;
    char * VendorData;
    int VendorDataLen;
};
#endif
