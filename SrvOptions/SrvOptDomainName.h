/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef SRVOPTDOMAINNAME_H
#define SRVOPTDOMAINNAME_H

#include <iostream>
#include "OptDomainName.h"

class TSrvOptDomainName : public TOptDomainName
{
public:
    TSrvOptDomainName(string domain, TMsg* parent);
    TSrvOptDomainName(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};
#endif
