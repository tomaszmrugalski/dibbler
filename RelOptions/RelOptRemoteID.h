/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TRelOptRemoteID;
#ifndef RELOPTREMOTEID_H
#define RELOPTREMOTEID_H

#include "OptVendorData.h"

class TRelOptRemoteID : public TOptVendorData
{
public:
    TRelOptRemoteID(int enterprise, char * data, int dataLen, TMsg* parent);
    TRelOptRemoteID(char * buf,  int n, TMsg* parent);
    bool doDuties();
 private:
};

#endif /* CLNTOPTVENDORSPECINFO_H */
