#ifndef IPV6ADDR_H
#define IPV6ADDR_H
#include <iostream>
#include <iomanip>
using namespace std;
class TIPv6Addr
{
    friend ostream& operator<<(ostream& out,TIPv6Addr& group);
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
