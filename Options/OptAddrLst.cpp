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
#include <sstream>
#include "Portable.h"
#include "OptAddrLst.h"
#include "DHCPConst.h"

TOptAddrLst::TOptAddrLst(int type, List(TIPv6Addr) lst, TMsg* parent)
    :TOpt(type, parent), AddrLst(lst) {
}

TOptAddrLst::TOptAddrLst(int type, const char* buf, unsigned short bufSize, TMsg* parent)
    :TOpt(type, parent)
{
    while(bufSize>0)
    {
	if (bufSize<16) {
	    Valid = false;
	    return;
	}
        this->AddrLst.append(new TIPv6Addr(buf));
	buf +=16;
	bufSize -= 16;
    }
    Valid = true;
    return;
}

char * TOptAddrLst::storeSelf(char* buf) {
    SPtr<TIPv6Addr> addr;

    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    AddrLst.first();
    while(addr=AddrLst.get())
	buf=addr->storeSelf(buf);
    return buf;
}

size_t TOptAddrLst::getSize()
{
    return 4+16*AddrLst.count();
}

void TOptAddrLst::firstAddr() {
    this->AddrLst.first();
}

SPtr<TIPv6Addr> TOptAddrLst::getAddr()
{
    return this->AddrLst.get();
}

bool TOptAddrLst::isValid() const
{
    return this->Valid;
}

std::string TOptAddrLst::getPlain() {
    std::stringstream tmp;
    AddrLst.first();
    SPtr<TIPv6Addr> addr;
    while (addr = AddrLst.get()) {
        tmp << addr->getPlain() << " ";
    }
    return tmp.str();
}
