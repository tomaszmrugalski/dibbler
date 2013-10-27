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
#include "hex.h"

#if defined(LINUX) || defined(BSD)
#include <arpa/inet.h>
#endif

using namespace std;

TOptVendorData::TOptVendorData(int type, int enterprise, char * data, int dataLen, TMsg* parent) 
    :TOpt(type, parent) {
    Vendor = enterprise;
    VendorData = new char[dataLen];
    memcpy(VendorData, data, dataLen);
    VendorDataLen = dataLen;
}

TOptVendorData::TOptVendorData(int type, const char * buf,  int n, TMsg* parent)
    :TOpt(type, parent)
{
    if (n<4) {
	Log(Warning) << "Unable to parse " << type << "option." << LogEnd;
	this->Vendor = 0;
	this->VendorData = 0;
	this->VendorDataLen = 0;
	return;
    }

    this->Vendor = readUint32(buf); // enterprise number
    buf += sizeof(uint32_t);
    n   -= sizeof(uint32_t);

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

size_t TOptVendorData::getSize() {
    return 8 + VendorDataLen; /* 8 normal header(4) + enterprise (4) */
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
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    buf = writeUint32(buf, this->Vendor);
    memmove(buf, this->VendorData, this->VendorDataLen);
    buf+=this->VendorDataLen;
    return buf;    
}

bool TOptVendorData::isValid() const
{
    return true;
}

int TOptVendorData::getVendor()
{
    return Vendor;
}

char * TOptVendorData::getVendorData()
{
    return VendorData;
}

std::string TOptVendorData::getVendorDataPlain()
{
    return hexToText((uint8_t*)&VendorData[0], VendorDataLen);
}

int TOptVendorData::getVendorDataLen()
{
    return VendorDataLen;
}

