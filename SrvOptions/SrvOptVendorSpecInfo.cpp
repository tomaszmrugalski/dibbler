/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#include "SrvOptVendorSpecInfo.h"

TSrvOptVendorSpecInfo::TSrvOptVendorSpecInfo( char * buf,  int n, TMsg* parent)
	:TOptVendorSpecInfo(buf,n, parent)
{

}

bool TSrvOptVendorSpecInfo::doDuties()
{
    return true;
}
