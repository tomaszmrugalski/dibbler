#ifndef CLNTSERVERIDENTIFIER_H
#define CLNTSERVERIDENTIFIER_H

#include "DHCPConst.h"
#include "OptServerIdentifier.h"

class TClntOptServerIdentifier : public TOptServerIdentifier 
{
  public:
    TClntOptServerIdentifier( char * duid, int n, TMsg* parent);
    TClntOptServerIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
	bool doDuties();
};
#endif
