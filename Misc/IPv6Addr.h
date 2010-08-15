/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: IPv6Addr.h,v 1.8 2007-05-03 23:16:29 thomson Exp $
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef IPV6ADDR_H
#define IPV6ADDR_H

#include <iostream>
#include <list>
#include <SmartPtr.h>

class TIPv6Addr
{
	friend std::ostream& operator<<(std::ostream& out,TIPv6Addr& group);
public:
    TIPv6Addr();    //Creates any address
    TIPv6Addr(const char* addr, bool plain=false);
    TIPv6Addr(const char* prefix, const char* host, int prefixLength); /* creates address from prefix+host */
    char* getAddr();
    void setAddr(char* addr);
    char* getPlain();
    char* storeSelf(char *buf);
    bool linkLocal();
    TIPv6Addr operator-(const TIPv6Addr &other);
    TIPv6Addr operator+(const TIPv6Addr &other);
    TIPv6Addr& operator--();
    bool operator==(const TIPv6Addr &other);
    bool operator<=(const TIPv6Addr &other);
    void truncate(int minPrefix, int maxPrefix);
    void shiftL(int bits);
    void shiftR(int bits);
private:
    char Addr[16];
    char Plain[sizeof("0000:0000:0000:0000:0000:0000:0000.000.000.000.000")];
};

typedef std::list< SPtr<TIPv6Addr> > TAddrLst;
#endif
