/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptVendorSpec.cpp,v 1.3 2008-03-02 19:19:10 thomson Exp $
 *
 */

#include "ClntOptVendorSpec.h"

TClntOptVendorSpec::TClntOptVendorSpec( char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, buf,n, parent)
{
}

TClntOptVendorSpec::TClntOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, enterprise, data, dataLen, parent)
{
}

bool TClntOptVendorSpec::doDuties() {
    return true;
}

void TClntOptVendorSpec::setIfaceMgr(SPtr<TClntIfaceMgr> ifaceMgr)
{
    this->IfaceMgr = IfaceMgr;
}
