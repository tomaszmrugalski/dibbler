/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptVendorSpec.cpp,v 1.3 2008-08-29 00:07:38 thomson Exp $
 */

#include "SrvOptVendorSpec.h"

TSrvOptVendorSpec::TSrvOptVendorSpec(char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, buf,n, parent)
{
    
}

TSrvOptVendorSpec::TSrvOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, enterprise, data, dataLen, parent)
{
}

bool TSrvOptVendorSpec::doDuties()
{
    return true;
}
