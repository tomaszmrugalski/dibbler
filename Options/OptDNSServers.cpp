#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
#include "OptDNSServers.h"
#include "DHCPConst.h"

TOptDNSServers::TOptDNSServers(TContainer<SmartPtr<TIPv6Addr> > DNSSrvLst, TMsg* parent)
    :TOpt(OPTION_DNS_RESOLVERS, parent), DNSSrv(DNSSrvLst)
{
    
}

TOptDNSServers::TOptDNSServers(char* &buf,int &bufSize, TMsg* parent)
    :TOpt(OPTION_DNS_RESOLVERS,parent)
{
    while(bufSize>0)
    {
	if (bufSize<16) {
	    Valid = false;
	    return;
	}
        this->DNSSrv.append(new TIPv6Addr(buf));
        bufSize-=16;
        buf+=16;
    }
    Valid = true;
    return;
}

char * TOptDNSServers::storeSelf(char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;
    if (DNSSrv.count())
    {
        SmartPtr<TIPv6Addr> addr;
        DNSSrv.first();
        while(addr=DNSSrv.get())
            buf=addr->storeSelf(buf);
    }
    else
        return buf;
    return buf;
}

int TOptDNSServers::getSize()
{
    if (DNSSrv.count())
        return 4+16*DNSSrv.count();
    else
        return 0;
}

void TOptDNSServers::firstDNSrv()
{
    this->DNSSrv.first();
}

SmartPtr<TIPv6Addr> TOptDNSServers::getDNSSrv()
{
    return this->DNSSrv.get();
}

bool TOptDNSServers::isValid()
{
    return this->Valid;
}
