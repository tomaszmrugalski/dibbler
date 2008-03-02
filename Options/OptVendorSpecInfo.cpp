/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptVendorSpecInfo.cpp,v 1.10 2008-03-02 19:19:43 thomson Exp $
 *
 */

#include <iostream>
#include <sstream>
#include "Portable.h"
#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"
#include "Logger.h"

TOptVendorSpecInfo::TOptVendorSpecInfo(int type, char * &buf,  int &n, TMsg* parent)
    :TOpt(type, parent)
{
    if (n<4) {
	Log(Error) << "Unable to parse vendor-spec info option." << LogEnd;
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

    buf += n;
    n    = 0;
}

TOptVendorSpecInfo::TOptVendorSpecInfo(int type, int enterprise, char *data, int dataLen, TMsg* parent)
    :TOpt(type, parent)
{
    this->Vendor = enterprise;
    this->VendorData = new char[dataLen];
    memmove(this->VendorData, data, dataLen);
    this->VendorDataLen = dataLen;
}

TOptVendorSpecInfo::~TOptVendorSpecInfo() 
{
    if (this->VendorDataLen)
	delete [] VendorData;
}

int TOptVendorSpecInfo::getSize()
{
    return 8+this->VendorDataLen; /* normal header(4) + enterprise(4) */
}

char * TOptVendorSpecInfo::storeSelf( char* buf)
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

bool TOptVendorSpecInfo::isValid()
{
    return true;
}

int TOptVendorSpecInfo::getVendor()
{
    return this->Vendor;
}

char * TOptVendorSpecInfo::getVendorData()
{
    return this->VendorData;
}

string TOptVendorSpecInfo::getVendorDataPlain()
{
    ostringstream tmp;
    tmp << "0x";
    for (int i=0; i<this->VendorDataLen; i++) {
	tmp << setfill('0') << setw(2) << hex << (unsigned int) this->VendorData[i];
    }
    return tmp.str();
}

int TOptVendorSpecInfo::getVendorDataLen()
{
    return this->VendorDataLen;
}
    
