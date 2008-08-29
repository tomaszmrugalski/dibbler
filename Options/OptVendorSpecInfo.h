/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
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
//    TOptVendorSpecInfo(int type, int number, char * data, int dataLen);
    TOptVendorSpecInfo(int type, char * &buf,  int &n,TMsg* parent);
    TOptVendorSpecInfo(int type, int enterprise, char *data, int dataLen, TMsg* parent);
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
    ~TOptVendorSpecInfo();
};

#endif /* OPTVENDORSPECINFO_H */
