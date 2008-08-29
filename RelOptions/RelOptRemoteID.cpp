/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelOptRemoteID.cpp,v 1.2 2008-08-29 00:07:33 thomson Exp $
 *
 */

#include "RelOptRemoteID.h"

TRelOptRemoteID::TRelOptRemoteID( char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_REMOTE_ID, buf,n, parent)
{
}

TRelOptRemoteID::TRelOptRemoteID(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_REMOTE_ID, enterprise, data, dataLen, parent)
{
}

bool TRelOptRemoteID::doDuties() {
    return true;
}
