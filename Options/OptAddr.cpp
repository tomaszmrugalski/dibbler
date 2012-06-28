/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "Portable.h"
#include "OptAddr.h"
#include "Logger.h"
#include "OptRtPrefix.h"
#include "OptGeneric.h"

TOptAddr::TOptAddr(int type, const char * buf, unsigned short len, TMsg* parent)
    :TOpt(type, parent) {
    if (len<16) {
	Valid = false;
	Log(Warning) << "Malformed option (code=" << type << ", length=" << len
		     << "), expected length is 16." << LogEnd;
	return;
    }
    Addr = new TIPv6Addr(buf, false); // plain = false
    buf += 16;
    len -= 16;

    Valid = parseOptions(SubOptions, buf, len, parent);
}

TOptAddr::TOptAddr(int type, SPtr<TIPv6Addr> addr, TMsg* parent)
    :TOpt(type, parent) {
    this->Addr = addr;
}

size_t TOptAddr::getSize() {
    // 20 - size of this option
    return 20 + getSubOptSize();
}

SPtr<TIPv6Addr> TOptAddr::getAddr() {
    return Addr;
}

std::string TOptAddr::getPlain() {
    return Addr->getPlain();
}

char * TOptAddr::storeSelf(char* buf) {
    // store generic header
    buf = writeUint16( buf, OptType );
    buf = writeUint16( buf, getSize() - 4 );

    // store address
    buf = Addr->storeSelf(buf);

    // store sub-options (if three are any)
    return storeSubOpt(buf);
}
