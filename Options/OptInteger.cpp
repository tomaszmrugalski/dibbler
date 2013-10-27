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
#include <iostream>
#include "Portable.h"
#include "OptInteger.h"
#include "DHCPConst.h"

using namespace std;

TOptInteger::TOptInteger(uint16_t type, unsigned int integerLen, unsigned int value, TMsg* parent)
    :TOpt(type, parent) {
    this->Value = value;
    this->Valid = true;
    this->Len   = integerLen;
}

TOptInteger::TOptInteger(uint16_t type, unsigned int len, const char *buf, size_t bufsize, TMsg* parent)
    :TOpt(type, parent)
{
    if ((unsigned int)bufsize<len) {
	this->Valid = false;
	return;
    }
    this->Len   = len;
    this->Valid = true;
    switch (len) {
    case 0:
	this->Value = 0;
	break;
    case 1:
	this->Value = (unsigned char)(*buf);
	break;
    case 2:
	this->Value = readUint16(buf);
	break;
    case 3:
	this->Value = ((long)buf[0])<<16 | ((long)buf[1])<<8 | (long)buf[2]; /* no ntoh3, eh? */
	break;
    case 4:
	this->Value = readUint32(buf);
	break;
    default:
	this->Valid = false;
	return;
    }
}

char * TOptInteger::storeSelf(char* buf)
{
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, this->Len);
    switch (this->Len) {
    case 0:
	break;
    case 1:
	*buf = (char)this->Value;
	break;
    case 2:
	buf = writeUint16(buf, this->Value);
	break;
    case 3:
    {
	int tmp = this->Value;
	buf[0] = tmp%256; tmp = tmp/256;
	buf[1] = tmp%256; tmp = tmp/256;
	buf[2] = tmp%256; tmp = tmp/256;
	break;
    }
    case 4:
        buf = writeUint32(buf, this->Value);
	break;
    default:
	/* this should never happen */
	break;
    }
    return buf+this->Len;    
}
size_t TOptInteger::getSize() {
    return 4 /*option header length*/ + this->Len; 
}

unsigned int TOptInteger::getValue() {
    return this->Value;
}

std::string TOptInteger::getPlain() {
    stringstream tmp;
    tmp << Value;
    return tmp.str();
}

bool TOptInteger::isValid() const {
    return this->Valid;
}
