#ifndef SRVOPTDNSSERVERS_H
#define SRVOPTDNSSERVERS_H

#include "OptDNSServers.h"

class TSrvOptDNSServers : public TOptDNSServers
{
public:
    TSrvOptDNSServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent);
    TSrvOptDNSServers(char* buf, int size, TMsg* parent);
    bool doDuties();
};
#endif