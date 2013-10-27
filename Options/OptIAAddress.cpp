/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

//#include <netinet/in.h>
#include <string.h>
#include "Portable.h"
#include "DHCPConst.h"
#include "Opt.h"
#include "OptIAAddress.h"

TOptIAAddress::TOptIAAddress(char * &buf, int& n, TMsg* parent)
    :TOpt(OPTION_IAADDR, parent), Valid_(false)
{
    if ( n >= 24) {
        Addr_ = new TIPv6Addr(buf);
        buf += 16;
        n -= 16;
        PrefLifetime_ = readUint32(buf);
        buf += sizeof(uint32_t);
        n -= sizeof(uint32_t);
        ValidLifetime_ = readUint32(buf);
        buf += sizeof(uint32_t);
        n -= sizeof(uint32_t);

        Valid_ = true;
    }
}

TOptIAAddress::TOptIAAddress(SPtr<TIPv6Addr> addr, unsigned long pref,
                             unsigned long valid, TMsg* parent)
    :TOpt(OPTION_IAADDR, parent), Valid_(true) {
    if(addr)
        Addr_ = addr;
    else
        Addr_ = new TIPv6Addr();
    PrefLifetime_  = pref;
    ValidLifetime_ = valid;
}

size_t TOptIAAddress::getSize() {
    int mySize = 28;
    return mySize + getSubOptSize();
}

void TOptIAAddress::setPref(unsigned long pref) {
    PrefLifetime_ = pref;
}

void TOptIAAddress::setValid(unsigned long valid) {
    ValidLifetime_ = valid;
}

 char * TOptIAAddress::storeSelf( char* buf)
{
    buf = writeUint16(buf, OptType);

    buf = writeUint16(buf, getSize() - 4 );

    memcpy(buf, Addr_->getAddr(), 16);
    buf += 16;

    buf = writeUint32(buf, PrefLifetime_);

    buf = writeUint32(buf, ValidLifetime_);

    buf = storeSubOpt(buf);

    return buf;
}

SPtr<TIPv6Addr> TOptIAAddress::getAddr() const {
    return Addr_;
}

unsigned long TOptIAAddress::getPref() const {
    return PrefLifetime_;
}

unsigned long TOptIAAddress::getValid() const {
    return ValidLifetime_;
}

bool TOptIAAddress::isValid() const {
    return Valid_;
}
