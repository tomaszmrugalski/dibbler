#ifndef SRVCLIENTIDENTIFIER_H_HEADER_INCLUDED_C10D9EA9
#define SRVCLIENTIDENTIFIER_H_HEADER_INCLUDED_C10D9EA9

#include "OptClientIdentifier.h"

class TOptClientIdentifier;

class TSrvOptClientIdentifier : public TOptClientIdentifier
{
  public:
    TSrvOptClientIdentifier( SmartPtr<TDUID> duid, TMsg* parent);
    TSrvOptClientIdentifier(char* buf, int bufsize, TMsg* parent);
	bool doDuties();
};

#endif /* CLIENTIDENTIFIER_H_HEADER_INCLUDED_C10D9EA9 */
