/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelOptRemoteID.h,v 1.2 2008-08-29 00:07:33 thomson Exp $
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
