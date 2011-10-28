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

TOptAddr::TOptAddr(int type, const char * buf, unsigned short len, TMsg* parent)
    :TOpt(type, parent) {
    if (len!=16) {
	Valid = false;
	Log(Warning) << "Malformed option (code=" << type << ", length=" << len
		     << "), expected length is 16." << LogEnd;
	return;
    }
    Addr = new TIPv6Addr(buf, false); // plain = false
}

TOptAddr::TOptAddr(int type, SPtr<TIPv6Addr> addr, TMsg* parent)
    :TOpt(type, parent) {
    this->Addr = addr;
}

int TOptAddr::getSize() {
    return 20;
}

SPtr<TIPv6Addr> TOptAddr::getAddr() {
    return Addr;
}

std::string TOptAddr::getPlain() {
    return Addr->getPlain();
}

char * TOptAddr::storeSelf(char* buf) {

    buf = writeUint16( buf, OptType );
    buf = writeUint16( buf, getSize() - 4 );
    return Addr->storeSelf(buf);
}
