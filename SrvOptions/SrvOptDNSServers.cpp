/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */
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
