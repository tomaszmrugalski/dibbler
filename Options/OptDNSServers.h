#ifndef OPTDNSSERVERS_H
#define OPTDNSSERVERS_H
#include "IPv6Addr.h"
#include "Container.h"
#include "SmartPtr.h"
#include "Opt.h"

class TOptDNSServers : public TOpt
{
public:
    TOptDNSServers(TContainer<SmartPtr<TIPv6Addr> > DNSsrvLst, TMsg* parent);
    TOptDNSServers(char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    void firstDNSrv();
    SmartPtr<TIPv6Addr> getDNSSrv();
    bool isValid();
protected:
    TContainer<SmartPtr<TIPv6Addr> > DNSSrv;
    bool Valid;

};
#endif