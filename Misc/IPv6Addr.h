/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: IPv6Addr.h,v 1.3 2004-03-29 22:06:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef IPV6ADDR_H
#define IPV6ADDR_H

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
class TIPv6Addr
{
	friend std::ostream& operator<<(std::ostream& out,TIPv6Addr& group);
public:
    TIPv6Addr();    //Creates any address
    TIPv6Addr(char* addr, bool plain=false);
    char* getAddr();
    void setAddr(char* addr);
    char* getPlain();
    char* storeSelf(char *buf);
    TIPv6Addr operator-(const TIPv6Addr &other);
    TIPv6Addr operator+(const TIPv6Addr &other);
    TIPv6Addr& operator--();
    bool operator==(const TIPv6Addr &other);
    bool operator<=(const TIPv6Addr &other);
private:
    char Addr[16];
    char Plain[sizeof("0000:0000:0000:0000:0000:0000:0000.000.000.000.000")];
};
#endif
