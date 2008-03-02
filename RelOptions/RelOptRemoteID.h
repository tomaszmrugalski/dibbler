/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelOptRemoteID.h,v 1.1 2008-03-02 19:32:28 thomson Exp $
 */

class TRelOptRemoteID;
#ifndef RELOPTREMOTEID_H
#define RELOPTREMOTEID_H

#include "OptVendorSpecInfo.h"

class TRelOptRemoteID : public TOptVendorSpecInfo
{
public:
    TRelOptRemoteID(int enterprise, char * data, int dataLen, TMsg* parent);
    TRelOptRemoteID(char * buf,  int n, TMsg* parent);
    bool doDuties();
 private:
};

#endif /* CLNTOPTVENDORSPECINFO_H */
