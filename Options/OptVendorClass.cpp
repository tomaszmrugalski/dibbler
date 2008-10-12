/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptVendorClass.cpp,v 1.4 2008-10-12 16:05:06 thomson Exp $
 *
 */
#include "OptVendorClass.h"
#include <iostream>
#include <sstream>
#include "Portable.h"
#include "DHCPConst.h"
#include "Logger.h"

TOptVendorClass::TOptVendorClass( char * &buf,  int &n, TMsg* parent)	:TOpt(OPTION_VENDOR_CLASS, parent){

    if (n<4) {
	Log(Error) << "Unable to parse vendor-class info option." << LogEnd;
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

int TOptVendorClass::getSize()
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
char * TOptVendorClass::storeSelf( char* buf)
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

bool TOptVendorClass::isValid()
{
    return true;
}

int TOptVendorClass::getVendor()
{
    return this->Vendor;
}

char * TOptVendorClass::getVendorData()
{
    return this->VendorData;
}

string TOptVendorClass::getVendorDataPlain()
{
    ostringstream tmp;
    tmp << "0x";
    for (int i=0; i<this->VendorDataLen; i++) {
	tmp << setfill('0') << setw(2) << hex << (unsigned int) this->VendorData[i];
    }
    return tmp.str();
}

int TOptVendorClass::getVendorDataLen()
{
    return this->VendorDataLen;
}

