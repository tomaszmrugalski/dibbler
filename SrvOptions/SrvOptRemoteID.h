/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#ifndef SRVOPTREMOTEID_H
#define SRVOPTREMOTEID_H

#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

class TSrvOptRemoteID : public TOptVendorSpecInfo
{
 public:
    TSrvOptRemoteID(int enterprise, char * data, int dataLen, TMsg* parent);
    TSrvOptRemoteID(char * buf,  int n, TMsg* parent);
    bool doDuties();
};

#endif /*  */
