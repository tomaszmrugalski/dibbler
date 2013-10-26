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
#include <string.h>
#include "Portable.h"
#include <sstream>
#include "DHCPConst.h"
#include "OptGeneric.h"
#include "hex.h"

using namespace std;

TOptGeneric::TOptGeneric(int optType, const char* data, unsigned short dataLen, TMsg* parent)
    :TOpt(optType, parent) {
    this->Data = new char[dataLen];
    memcpy(this->Data, data, dataLen);
    this->DataLen = dataLen;
}

TOptGeneric::TOptGeneric(int optType, TMsg * parent)
    :TOpt(optType, parent) {
    this->Data = 0;
    this->DataLen = 0;
}

TOptGeneric::~TOptGeneric() {
    if (this->DataLen) {
	delete [] this->Data;
    }
}

size_t TOptGeneric::getSize()
{
    return DataLen + 4;
}

char * TOptGeneric::storeSelf(char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, this->DataLen);
    memmove(buf, this->Data, this->DataLen);
    return buf+this->DataLen;
}

std::string TOptGeneric::getPlain()
{
    return hexToText((uint8_t*)(Data), DataLen, false);
}

bool TOptGeneric::isValid() const
{
    return true;
}

bool TOptGeneric::operator == (const TOptGeneric &other) {
    /// @todo: Implement comparison for real
    return false;
}
