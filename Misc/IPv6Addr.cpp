/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: IPv6Addr.cpp,v 1.11 2008-08-29 00:07:30 thomson Exp $
 *
 */

#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "IPv6Addr.h"
#include "Portable.h"
#include "Logger.h"

static char truncLeft[] = { 0xff, 0x7f, 0x3f, 0x1f, 0xf,  0x7,  0x3,  0x1, 0 };
static char truncRight[]= { 0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

TIPv6Addr::TIPv6Addr() {
    memset(Addr,0,16);
    inet_ntop6(Addr,Plain);
}

TIPv6Addr::TIPv6Addr(const char* addr, bool plain) {
    if (plain) {
        strcpy(Plain,addr);
        inet_pton6(Plain,Addr);
    } else {
        memcpy(Addr,addr,16);
	inet_ntop6(Addr,Plain);
    }
}

TIPv6Addr::TIPv6Addr(const char* prefix, const char* host, int prefixLength) {

    int offset = prefixLength/8;
    if (prefixLength%8==0) {
	memmove(Addr, host, 16);
	memmove(Addr, prefix, offset);
	inet_ntop6(Addr, Plain);
	return;
    }

    memmove(Addr, host, 16);  // copy whole host address, but...
    memmove(Addr, prefix, offset); // overwrite first bits with prefix...
    Addr[offset] = (prefix[offset] & truncRight[prefixLength%8]) | (host[offset] & truncLeft[prefixLength%8]);
    inet_ntop6(Addr, Plain);
}

bool TIPv6Addr::linkLocal() {
    if (this->Addr[0]==0xfe &&
	this->Addr[1]==0x80)
	return true;
    return false;
}

char* TIPv6Addr::getAddr() {
    return Addr;
}

char* TIPv6Addr::getPlain() {
    inet_ntop6( Addr, Plain);
    return Plain;
}

void TIPv6Addr::setAddr(char* addr) {
    memcpy(Addr,addr,16);
    inet_ntop6(Addr,Plain);
}

char* TIPv6Addr::storeSelf(char *buf) {
    memcpy(buf,this->Addr,16);
    return buf+16;
}

bool TIPv6Addr::operator==(const TIPv6Addr &other) {
    return !memcmp(this->Addr,other.Addr,16);
}

void TIPv6Addr::truncate(int minPrefix, int maxPrefix) {

    if (minPrefix>128 || minPrefix<0 || maxPrefix>128 || maxPrefix<0) {
	Log(Error) << "Unable to truncate address: invalid prefix lengths: minPrefix=" << minPrefix << ", maxPrefix=" << maxPrefix << LogEnd;
	return;
    }

    // truncating from the left 
    int x = minPrefix/8;
    memset(this->Addr, 0, x);
    if (minPrefix%8) {
	this->Addr[x] = this->Addr[x] & truncLeft[minPrefix%8];
    }

    // truncating from the right
    x = maxPrefix/8;
    if (maxPrefix%8)
	x++;
    memset(this->Addr+x, 0, 16-x);
    if (maxPrefix%8) {
	x = maxPrefix/8;
	this->Addr[x] = this->Addr[x] & truncRight[maxPrefix%8];
    }

    // update plain form
    inet_ntop6(Addr,Plain);
}

void TIPv6Addr::shiftL(int bits)
{

}

void TIPv6Addr::shiftR(int bits)
{

}

std::ostream& operator<<(std::ostream& out,TIPv6Addr& addr)
{
	char buf[48];
	if (addr.Addr) {
		inet_ntop6(addr.Addr, buf);
		out << buf;
	} else {
		out << "::";
	}
    return out;
}

bool TIPv6Addr::operator<=(const TIPv6Addr &other)
{
    for (int i=0;i<16;i++) {
        if (Addr[i]<other.Addr[i])
            return true;
        if(Addr[i]>other.Addr[i])
	    return false;
    }
    return true; //hmm: are they equal
}

TIPv6Addr TIPv6Addr::operator-(const TIPv6Addr &other)
{   
    char result[16];
    memset(result,0,16);
	char carry=0;
    for (int i=15;i>=0;i--)
    {
        unsigned int left=Addr[i];
        unsigned int right=other.Addr[i];
        if(left>=(right+carry))
        {
            result[i]=left-other.Addr[i]-carry;
            carry=0;
        }
        else
        {
            result[i]=Addr[i]+256-other.Addr[i]-carry;
            carry=1;
        }
    }
    return TIPv6Addr(result);
}

TIPv6Addr TIPv6Addr::operator+(const TIPv6Addr &other)
{   
    char result[16];
    memset(result,0,16);
    unsigned int carry=0;
    for (int i=15;i>=0;i--)
    {
        unsigned int left=Addr[i];
        unsigned int right=other.Addr[i];
        if(left+right+carry>255)
        {
            result[i]=char (left+right+carry-256);
            carry=1;
        }
        else
        {
            result[i]=left+right+carry;
            carry=0;
        }
    }
    return TIPv6Addr(result);
}

/** 
 *  Decreases randomly an address
 *  
 * 
 * @return 
 */
TIPv6Addr& TIPv6Addr::operator--()
{
    for (int i=15;i>=0;i--)
    {
        int j=i-1;
        while((j>=0)&&(!Addr[j])) j--;
        int r;
        if (j>0) r=rand()%256;
        else r=rand()%(Addr[i]+1);
        if (r>Addr[i])
        {
            Addr[j]--;
            for(j++;j<i;j++) Addr[j]=255;
            Addr[i]=char(int(256)+int(Addr[i])-r);
        }
        else Addr[i]=r;
    }
    return *this;
}
