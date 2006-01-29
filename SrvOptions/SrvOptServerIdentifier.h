#ifndef SRVOPTSERVERIDENTIFIER_H
#define SRVOPTSERVERIDENTIFIER_H

#include "DHCPConst.h"
#include "OptDUID.h"

class TSrvOptServerIdentifier : public TOptDUID
{
  public:
    TSrvOptServerIdentifier( SmartPtr<TDUID> duid, TMsg* parent);
    TSrvOptServerIdentifier(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif 
