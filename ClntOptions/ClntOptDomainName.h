/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef CLNTOPTDOMAINNAME_H
#define CLNTOPTDOMAINNAME_H

#include <iostream>
#include "OptDomainName.h"
#include "DUID.h"
#include "SmartPtr.h"

class TClntOptDomainName : public TOptDomainName
{
public:
    TClntOptDomainName(string domain, TMsg* parent);
    TClntOptDomainName(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
