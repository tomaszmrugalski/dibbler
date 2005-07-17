#ifndef CLNTCLIENTIDENTIFIER_H_HEADER_INCLUDED_C10D9EA9
#define CLNTCLIENTIDENTIFIER_H_HEADER_INCLUDED_C10D9EA9

#include "OptClientIdentifier.h"
class TOptClientIdentifier;

//##ModelId=3EC75CE40007
class TClntOptClientIdentifier : public TOptClientIdentifier
{
  public:
    //##ModelId=3EFF0A8E0399
    TClntOptClientIdentifier(char * duid,int n, TMsg* parent);
    TClntOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
    bool doDuties();
};

#endif /* CLIENTIDENTIFIER_H_HEADER_INCLUDED_C10D9EA9 */
