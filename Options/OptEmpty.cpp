/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */
#include <stdlib.h>
#include "Portable.h"
#include "OptEmpty.h"

TOptEmpty::TOptEmpty(int code, const char * buf,  int n, TMsg* parent)
    :TOpt(code, parent) {
    if (n) {
	Valid = false;
    }
}

TOptEmpty::TOptEmpty(int code, TMsg* parent)
    :TOpt(code, parent) {
}

size_t TOptEmpty::getSize() {
    return 4;
}

 char * TOptEmpty::storeSelf( char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    return buf;
}
