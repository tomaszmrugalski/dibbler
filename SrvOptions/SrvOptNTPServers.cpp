#include "SrvOptNTPServers.h"

TSrvOptNTPServers::TSrvOptNTPServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent)
    :TOptNTPServers(lst, parent)
{

}

TSrvOptNTPServers::TSrvOptNTPServers(char* buf, int size, TMsg* parent)
    :TOptNTPServers(buf,size,parent)
{
    
}

bool TSrvOptNTPServers::doDuties()
{
    return true;
}
