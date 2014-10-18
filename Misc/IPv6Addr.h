/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef IPV6ADDR_H
#define IPV6ADDR_H

#include <list>
#include <SmartPtr.h>

class TIPv6Addr
{
        friend std::ostream& operator<<(std::ostream& out,TIPv6Addr& group);
public:
    TIPv6Addr();    //Creates any address
    TIPv6Addr(const char* addr, bool plain=false);
    /* creates address from prefix+host, used in SrvCfgPD */
    TIPv6Addr(const char* prefix, const char* host, int prefixLength);
    char* getAddr();
    void setAddr(char* addr);
    char* getPlain();
    char* storeSelf(char *buf);
    bool linkLocal();
    bool multicast();
    TIPv6Addr operator-(const TIPv6Addr &other);
    TIPv6Addr operator+(const TIPv6Addr &other);
    TIPv6Addr& operator--();
    TIPv6Addr& operator++();
    bool operator==(const TIPv6Addr &other);
    bool operator!=(const TIPv6Addr &other);
    bool operator<=(const TIPv6Addr &other);
    void truncate(int minPrefix, int maxPrefix);
private:
    char Addr[16];
    char Plain[sizeof("0000:0000:0000:0000:0000:0000:0000.000.000.000.000")];
};

typedef std::list< SPtr<TIPv6Addr> > TAddrLst;
#endif
