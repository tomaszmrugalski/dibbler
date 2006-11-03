/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptVendorSpec.cpp,v 1.1 2006-11-03 19:42:41 thomson Exp $
 *
 */

#include "ClntOptVendorSpec.h"

TClntOptVendorSpec::TClntOptVendorSpec( char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(buf,n, parent)
{

}

TClntOptVendorSpec::TClntOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(enterprise, data, dataLen, parent)
{
}

bool TClntOptVendorSpec::doDuties() {
    return true;
}
