/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptGeneric.cpp,v 1.2 2005-01-24 00:42:57 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/12 00:01:06  thomson
 * *** empty log message ***
 *
 */

#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif 
#ifdef WIN32
#include <winsock2.h>
#endif

#include "DHCPConst.h"
#include "OptGeneric.h"

TOptGeneric::TOptGeneric(int optType, char* data, int dataLen, TMsg* parent)
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

int TOptGeneric::getSize()
{
    return this->DataLen+4;
}

 char * TOptGeneric::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(this->DataLen);
    buf+=2;
    memmove(buf, this->Data, this->DataLen);
    return buf+this->DataLen;
}

bool TOptGeneric::isValid()
{
    return true;
}
