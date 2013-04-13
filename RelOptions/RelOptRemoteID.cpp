/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "RelOptRemoteID.h"
#include "DHCPConst.h"

TRelOptRemoteID::TRelOptRemoteID( char * buf,  int n, TMsg* parent)
    :TOptVendorData(OPTION_REMOTE_ID, buf,n, parent)
{
}

TRelOptRemoteID::TRelOptRemoteID(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorData(OPTION_REMOTE_ID, enterprise, data, dataLen, parent)
{
}

bool TRelOptRemoteID::doDuties() {
    return true;
}
