/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: IPv6Addr.h,v 1.8 2007-05-03 23:16:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2006-12-04 23:33:11  thomson
 * Prefix delegation related fixes
 *
 * Revision 1.6  2006-12-02 14:54:45  thomson
 * IPv6Addr::truncate implemented, inet_pton6 does not use IPv4-eccapsulated form
 *
 * Revision 1.5  2006-08-21 22:52:40  thomson
 * Various fixes.
 *
 * Revision 1.4  2004/12/07 20:53:14  thomson
 * Link local safety checks added (bug #39)
 *
 * Revision 1.3  2004/03/29 22:06:49  thomson
 * 0.1.1 version
 *
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
#endif
