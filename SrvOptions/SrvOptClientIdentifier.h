#ifndef SRVCLIENTIDENTIFIER_H
#define SRVCLIENTIDENTIFIER_H

#include "OptDUID.h"

class TOptDUID;

class TSrvOptClientIdentifier : public TOptDUID
{
 public:
    TSrvOptClientIdentifier( SmartPtr<TDUID> duid, TMsg* parent);
    TSrvOptClientIdentifier(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif
