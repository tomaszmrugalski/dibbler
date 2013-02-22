/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "RelOptRemoteID.h"
#include "Portable.h"
#include "DHCPConst.h"

TRelOptRemoteID::TRelOptRemoteID( char * buf,  int n, TMsg* parent)
    :TOptVendorData(OPTION_REMOTE_ID, buf,n, parent)
{
}

TRelOptRemoteID::TRelOptRemoteID(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorData(OPTION_REMOTE_ID, enterprise, data, dataLen, parent)
{
}

bool TRelOptRemoteID::doDuties() {
    return true;
}

char * TRelOptRemoteID::storeSelf(char *buf)
{
    // option-code OPTION_VENDOR_OPTS (2 bytes long)
    buf = writeUint16(buf, OptType);

    // option-len size of total option-data
    buf = writeUint16(buf, getSize()-4);

    // enterprise-number (4 bytes long)
    buf = writeUint32(buf, this->Vendor);

    SPtr<TOpt> opt;
    firstOption();

    while (opt = getOption())
    {
        buf = opt->storeSelf(buf);
    }

    return buf;
}
