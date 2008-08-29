/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "SrvOptVendorClass.h"

TSrvOptVendorClass::TSrvOptVendorClass( char * buf,  int n, TMsg* parent)
	:TOptVendorClass(buf,n, parent)
{
	
}
bool TSrvOptVendorClass::doDuties()
{
    return true;
}
