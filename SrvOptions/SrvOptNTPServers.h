#ifndef SRVOPTNTPSERVERS_H
#define SRVOPTNTPSERVERS_H
#include "OptNTPServers.h"

class TSrvOptNTPServers : public TOptNTPServers
{
public:
    TSrvOptNTPServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent);
    TSrvOptNTPServers(char* buf, int size, TMsg* parent);
    bool doDuties();
};
#endif