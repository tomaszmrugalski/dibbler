#ifndef OPTNTPSERVERS_H
#define OPTNTPSERVERS_H
#include "IPv6Addr.h"
#include "Container.h"
#include "SmartPtr.h"
#include "Opt.h"

class TOptNTPServers : public TOpt
{

public:
    TOptNTPServers(TContainer<SmartPtr<TIPv6Addr> > NTPsrvLst, TMsg* parent);
    TOptNTPServers(char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    void firstNTPSrv();
    SmartPtr<TIPv6Addr> getNTPSrv();
    bool isValid();
protected:
    TContainer<SmartPtr<TIPv6Addr> > NTPSrv;
    bool Valid;
};

#endif