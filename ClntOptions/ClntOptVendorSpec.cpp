/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 *
 */

#include "Logger.h"
#include "ClntOptVendorSpec.h"

TClntOptVendorSpec::TClntOptVendorSpec( char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, buf,n, parent)
{
}

TClntOptVendorSpec::TClntOptVendorSpec(int enterprise, int optionCode, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, enterprise, data, dataLen, parent)
{
}

bool TClntOptVendorSpec::doDuties() {
    return true;
}
