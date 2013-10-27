/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "DHCPConst.h"
#include "OptVendorClass.h"
#include "Portable.h"
#include <string.h>

TOptVendorClass::TOptVendorClass(uint16_t type, const char* buf, unsigned short buf_len, TMsg* parent)
    :TOptUserClass(type, parent) {
    if (buf_len < 4) {
	Valid = false;
	return;
    }
    Enterprise_id_ = readUint32(buf);
    buf += sizeof(uint32_t);
    buf_len -= sizeof(uint32_t);
    Valid = parseUserData(buf, buf_len);
}

size_t TOptVendorClass::getSize() {
    return 4 + TOptUserClass::getSize();
}

char * TOptVendorClass::storeSelf(char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4);
    buf = writeUint32(buf, Enterprise_id_);
    return storeUserData(buf);
}
