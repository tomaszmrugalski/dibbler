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