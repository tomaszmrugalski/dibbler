/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#ifndef CLNTVENDORSPEC_H
#define CLNTVENDORSPEC_H

#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

class TClntOptVendorSpec : public TOptVendorSpecInfo
{
public:
    TClntOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent);
    TClntOptVendorSpec(char * buf,  int n, TMsg* parent);
    bool doDuties();
    
};

#endif /* VENDORSPECINFO_H */
