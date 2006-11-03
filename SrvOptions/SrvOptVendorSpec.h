/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef SRVOPTVENDORSPECINFO_H
#define SRVOPTVENDORSPECINFO_H

#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

class TSrvOptVendorSpec : public TOptVendorSpecInfo
{
 public:
    TSrvOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent);
    TSrvOptVendorSpec(char * buf,  int n, TMsg* parent);
    bool doDuties();
};

#endif /* VENDORSPECINFO_H_HEADER_INCLUDED_C1123F31 */
