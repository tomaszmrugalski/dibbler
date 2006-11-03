/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptVendorSpec.h,v 1.2 2006-11-03 20:07:06 thomson Exp $
 */

#ifndef CLNTOPTVENDORSPEC_H
#define CLNTOPTVENDORSPEC_H

#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

class TClntOptVendorSpec : public TOptVendorSpecInfo
{
public:
    TClntOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent);
    TClntOptVendorSpec(char * buf,  int n, TMsg* parent);
    bool doDuties();
    
};

#endif /* CLNTOPTVENDORSPECINFO_H */
