#include "SrvOptDNSServers.h"

TSrvOptDNSServers::TSrvOptDNSServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent)
    :TOptDNSServers(lst, parent)
{

}

TSrvOptDNSServers::TSrvOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptDNSServers(buf,size, parent)
{
    
}

bool TSrvOptDNSServers::doDuties()
{    
    return true;
}
