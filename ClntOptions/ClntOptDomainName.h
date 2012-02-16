/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTOPTDOMAINNAME_H
#define CLNTOPTDOMAINNAME_H

#include <iostream>
#include "OptDomainLst.h"
#include "DUID.h"
#include "SmartPtr.h"

class TClntOptDomainName : public TOptDomainLst
{
 public:
    TClntOptDomainName(List(string) * domains, TMsg* parent);
    TClntOptDomainName(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SPtr<TDUID> duid);
private:
    SPtr<TDUID> SrvDUID;
};
#endif
