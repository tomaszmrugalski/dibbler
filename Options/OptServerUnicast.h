#ifndef SERVERUNICAST_H_HEADER_INCLUDED_C1126AA1
#define SERVERUNICAST_H_HEADER_INCLUDED_C1126AA1

#include "Opt.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"

class TOptServerUnicast : public TOpt
{
public:
    TOptServerUnicast( char * &buf,  int &n, TMsg* parent);
    int getSize();
    char * storeSelf( char* buf);
    SmartPtr<TIPv6Addr> getAddr();
private:
    SmartPtr<TIPv6Addr> Addr;
};

#endif
