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