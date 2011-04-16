/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <string.h>
#include "OptVendorData.h"
#include <iostream>
#include <sstream>
#include "Portable.h"
#include "DHCPConst.h"
#include "Logger.h"

TOptVendorData::TOptVendorData(int type, char * buf,  int n, TMsg* parent)
    :TOpt(type, parent)
{
    if (n<4) {
	Log(Warning) << "Unable to parse " << type << "option." << LogEnd;
	this->Vendor = 0;
	this->VendorData = 0;
	this->VendorDataLen = 0;
	return;
    }

    this->Vendor = ntohl(*(int*)buf); // enterprise number
    buf += 4;
    n   -= 4;

    if (!n) {
	this->VendorData = 0;
	this->VendorDataLen = 0;
	return;
    }

    if (n) {
	this->VendorData = new char[n];
	memmove(this->VendorData, buf, n);
    } else {
	this->VendorData = 0;
    }
    this->VendorDataLen = n;
}

int TOptVendorData::getSize()
{
    return 8+VendorDataLen; /* 8 normal header(4) + enterprise (4) */
}

/** 
 * stores option in a buffer
 * 
 * @param buf option will be stored here
 * 
 * @return pointer to the next unused byte
 */
char * TOptVendorData::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    *(uint32_t*)buf = htonl(this->Vendor);
    buf+=4;
    memmove(buf, this->VendorData, this->VendorDataLen);
    buf+=this->VendorDataLen;
    return buf;    
}

bool TOptVendorData::isValid()
{
    return true;
}

int TOptVendorData::getVendor()
{
    return Vendor;
}

char * TOptVendorData::getVendorData()
{
    return this->VendorData;
}

string TOptVendorData::getVendorDataPlain()
{
    ostringstream tmp;
    tmp << "0x";
    for (int i=0; i<this->VendorDataLen; i++) {
	tmp << setfill('0') << setw(2) << hex << (unsigned int) this->VendorData[i];
    }
    return tmp.str();
}

int TOptVendorData::getVendorDataLen()
{
    return this->VendorDataLen;
}

