/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptAddr.cpp,v 1.1 2004-10-26 22:36:57 thomson Exp $
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif
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

char * TOptAddr::storeSelf(char* buf) {
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    this->Addr->storeSelf(buf);
    return buf;
}
