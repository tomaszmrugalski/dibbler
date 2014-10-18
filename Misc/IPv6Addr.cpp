/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "IPv6Addr.h"
#include "Portable.h"
#include "Logger.h"

static unsigned char truncLeft[] = { 0xff, 0x7f, 0x3f, 0x1f, 0xf,  0x7,  0x3,  0x1, 0 };
static unsigned char truncRight[]= { 0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

TIPv6Addr::TIPv6Addr() {
    memset(Addr,0,16);
    inet_ntop6(Addr,Plain);
}

TIPv6Addr::TIPv6Addr(const char* addr, bool plain) {
    if (plain) {
        strncpy(Plain,addr, sizeof(Plain));
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

bool TIPv6Addr::multicast() {
    return Addr[0] == 0xff;
}


char* TIPv6Addr::getAddr() {
    return Addr;
}

char* TIPv6Addr::getPlain() {
    inet_ntop6(Addr, Plain);
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

bool TIPv6Addr::operator!=(const TIPv6Addr &other) {
    return memcmp(this->Addr,other.Addr,16);
}

void TIPv6Addr::truncate(int minPrefix, int maxPrefix) {

    if (minPrefix>128 || minPrefix<0 || maxPrefix>128 || maxPrefix<0) {
        Log(Error) << "Unable to truncate address: invalid prefix lengths: minPrefix="
                   << minPrefix << ", maxPrefix=" << maxPrefix << LogEnd;
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

std::ostream& operator<<(std::ostream& out,TIPv6Addr& addr)
{
    char buf[48];
    inet_ntop6(addr.Addr, buf);
    out << buf;
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
//#define ALGO_ANIA
//#define OLD_CRAPPY_CODE
//#define NEW_CRAPPY_CODE
#define NEW_AWESOME_CODE

#ifdef ALGO_ANIA
    // Start with first non-zero most significant byte.
    // For i-th byte randomize a value from 0..Addr[i]
    // If randomized value equals i-th (randomized max allowed
    // value, then continue)
    // If randomized value is smaller than i-th byte, then
    // all following bytes are random.
    //
    // Issue: for 1:: half of the addresses are 1::
    bool any = false; // insert any (0-255) value?
    for (int i=0; i<16; ++i) {

        if (!any) {
            // Let's search for first non-zero byte
            if (Addr[i] == 0)
                continue;

            // let's random a number from 0 to Addr[i]
            uint8_t x = random()%( (uint16_t)(Addr[i]) + 1);
            if (x < Addr[i]) {
                Addr[i] = x; // decrease this byte
                any = true; // next bytes are random
            }
        } else {
            // Completely random value
            Addr[i] = random() % 256;
        }
    }
#endif

#ifdef OLD_CRAPPY_CODE
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
            for(j++;j<i;j++)
                                Addr[j]=255;
            Addr[i]=char(int(256)+int(Addr[i])-r);
        }
        else Addr[i]=r;
    }
    return *this;
#endif

#ifdef NEW_CRAPPY_CODE

    // Let's iterate over all octects, starting with the least significant
    for (int i=15; i>=0; --i)
    {
        int j=i-1;
        while( (j>=0) && (!Addr[j]))
            j--;

        if (j == i - 1) {
            // this is the last non-zero byte
            Addr[i] = random()%(Addr[i] + 1);
        } else {

        }
        if (j < 0) {
            // there are no non-zero bytes left of current position

            // If i-th address is non-zero, let's decrease it randomly
            if (Addr[i]) {
                Addr[i] = rand()%(Addr[i] + 1);
            }
            return *this;
        }

        // Let's decrease this byte by a random value
        int16_t r = random()%256;

        // we try to decrease n-th byte by value greater than that byte,
        // so we need to borrow from n-1-th byte
        j = i;
        while (r > Addr[i] && j>=0) {

            // Borrow one from the next byte (it becomes 256 in this byte)
            Addr[j] = static_cast<uint8_t>( (int16_t)(256) + (int16_t)(Addr[i]) - r);
            r = 1; // subtract from the next
            j--;
        }
        if (j >= 0) {
            Addr[j] = Addr[j] - (uint8_t)(r); // decrease this byte
        }
    }
#endif

#ifdef NEW_AWESOME_CODE

    int j = 0;     // j - the most significant non-zero byte
    for (j = 0; j<15; j++) {
        if (Addr[j])
            break;
    }

    uint8_t b = 0; // Borrow from the next byte
    // Let's iterate over all octects, starting with the least significant
    for (int i=15; i>=0; --i)
    {
        // Did we underflow (subtract below zero) this byte?
        if (Addr[i] < b) {
            // Yes - borrow 256 from the next byte
            Addr[i] = Addr[i] - b;
            b = 1;
        } else {
            // No - we don't need to borrow anything
            Addr[i] = Addr[i] - b;
            b = 0;
        }

        if (j == i) {
            // this is the last non-zero byte
            unsigned int div = (unsigned int)(Addr[i]) + 1;
            Addr[i] = rand()%(div);
            return *this;
        }

        // Let's decrease this byte by a random value
        short int r = rand() % 256;

        // Do we need to borrow 256 from the next byte?
        b += (r > Addr[i]);

        r = (uint8_t)(-r + 256 + Addr[i]);
        Addr[i] = (uint8_t)(r);
    }
#endif

    return *this;
}

/// @brief increases address by one
///
TIPv6Addr& TIPv6Addr::operator++()
{
    int carry = 1;
    for (int i=15; i>=0; i--) {
        carry = (Addr[i] == 255);
        Addr[i]++;
        if (!carry)
            return *this;
    }

    return *this;
}
