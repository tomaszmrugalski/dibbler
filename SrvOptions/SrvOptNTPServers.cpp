/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

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
