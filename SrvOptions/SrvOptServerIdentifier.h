#ifndef SRVSERVERIDENTIFIER_H_HEADER_INCLUDED_C10DCE82
#define SRVSERVERIDENTIFIER_H_HEADER_INCLUDED_C10DCE82

#include "DHCPConst.h"
#include "OptServerIdentifier.h"

class TSrvOptServerIdentifier : public TOptServerIdentifier 
{
  public:
    TSrvOptServerIdentifier( SmartPtr<TDUID> duid, TMsg* parent);
    TSrvOptServerIdentifier(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};



#endif /* SERVERIDENTIFIER_H_HEADER_INCLUDED_C10DCE82 */
