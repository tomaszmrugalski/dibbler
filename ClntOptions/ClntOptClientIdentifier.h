#ifndef CLNTOPTCLIENTIDENTIFIER_H
#define CLNTOPTCLIENTIDENTIFIER_H

#include "OptDUID.h"

class TClntOptClientIdentifier : public TOptDUID
{
  public:
    TClntOptClientIdentifier(char * duid,int n, TMsg* parent);
    TClntOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
    bool doDuties();
};

#endif /* CLNTOPTCLIENTTIDENTIFIER_H */
