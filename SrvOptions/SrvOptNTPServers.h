/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

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
