/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

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
