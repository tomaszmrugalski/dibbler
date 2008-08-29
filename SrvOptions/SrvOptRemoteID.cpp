/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptRemoteID.cpp,v 1.2 2008-08-29 00:07:37 thomson Exp $
 */

#include "SrvOptRemoteID.h"

TSrvOptRemoteID::TSrvOptRemoteID(char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_REMOTE_ID, buf,n, parent)
{
    
}

TSrvOptRemoteID::TSrvOptRemoteID(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_REMOTE_ID, enterprise, data, dataLen, parent)
{
}

bool TSrvOptRemoteID::doDuties()
{
    return true;
}
