#ifndef CLNTOPTNTPSERVERS_H
#define CLNTOPTNTPSERVERS_H
#include "OptNTPServers.h"
#include "DUID.h"
#include "SmartPtr.h"

class TClntOptNTPServers : public TOptNTPServers
{
public:
    TClntOptNTPServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent);
    TClntOptNTPServers(char* buf, int size, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif