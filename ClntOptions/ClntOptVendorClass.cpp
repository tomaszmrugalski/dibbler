/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "ClntOptVendorClass.h"

TClntOptVendorClass::TClntOptVendorClass( char * buf,  int n, TMsg* parent)
    :TOptVendorData(OPTION_VENDOR_CLASS, buf, n, parent)
{
	
}
bool TClntOptVendorClass::doDuties()
{
    return false;
}
