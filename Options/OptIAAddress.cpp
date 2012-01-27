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
	:TOpt(OPTION_IAADDR, parent)
{
    ValidOpt=false;
    if (n>=24)
    {
        Addr = new TIPv6Addr(buf);
        buf += 16; n -= 16;
        Pref = readUint32(buf);
        buf += sizeof(uint32_t); n -= sizeof(uint32_t);
        Valid = readUint32(buf);
        buf += sizeof(uint32_t); n -= sizeof(uint32_t);

        ValidOpt=true;
    }
}

TOptIAAddress::TOptIAAddress(SPtr<TIPv6Addr> addr, unsigned long pref, 
			     unsigned long valid, TMsg* parent)
    :TOpt(OPTION_IAADDR, parent) {
    if(addr)
        Addr=addr;
    else
        Addr=new TIPv6Addr();
    this->Pref = pref;
    this->Valid = valid;
}

int TOptIAAddress::getSize() {
    int mySize = 28;
    return mySize+getSubOptSize();
}

void TOptIAAddress::setPref(unsigned long pref) {
    this->Pref = pref;
}

void TOptIAAddress::setValid(unsigned long valid) {
    this->Valid = valid;
}

 char * TOptIAAddress::storeSelf( char* buf)
{
    buf = writeUint16(buf, OptType);

    buf = writeUint16(buf, getSize() - 4 );

    memcpy(buf,Addr->getAddr(),16);
    buf+=16;

    buf = writeUint32(buf, Pref);

    buf = writeUint32(buf, Valid);

    buf=storeSubOpt(buf);

    return buf;
}

 SPtr<TIPv6Addr> TOptIAAddress::getAddr()
{
	return this->Addr;
}

unsigned long TOptIAAddress::getPref()
{
	return this->Pref;
}

unsigned long TOptIAAddress::getValid()
{
	return this->Valid;
}

bool TOptIAAddress::isValid()
{
    return this->ValidOpt;
}
