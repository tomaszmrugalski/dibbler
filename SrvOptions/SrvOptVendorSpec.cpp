/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptVendorSpec.cpp,v 1.1 2006-11-03 20:07:07 thomson Exp $
 */

#include "SrvOptVendorSpec.h"

TSrvOptVendorSpec::TSrvOptVendorSpec(char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(buf,n, parent)
{
    
}

TSrvOptVendorSpec::TSrvOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(enterprise, data, dataLen, parent)
{
}

bool TSrvOptVendorSpec::doDuties()
{
    return true;
}
