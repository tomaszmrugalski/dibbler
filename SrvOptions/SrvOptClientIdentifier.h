#ifndef SRVCLIENTIDENTIFIER_H
#define SRVCLIENTIDENTIFIER_H

#include "OptClientIdentifier.h"

class TOptClientIdentifier;

class TSrvOptClientIdentifier : public TOptClientIdentifier
{
 public:
    TSrvOptClientIdentifier( SmartPtr<TDUID> duid, TMsg* parent);
    TSrvOptClientIdentifier(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif
