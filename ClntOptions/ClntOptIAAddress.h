#ifndef CLNTIAADDRESS_H_HEADER_INCLUDED_C112313F
#define CLNTIAADDRESS_H_HEADER_INCLUDED_C112313F

#include "SmartPtr.h"
#include "Container.h"
#include "OptIAAddress.h"

class TClntOptIAAddress : public TOptIAAddress
{
 public:
    TClntOptIAAddress(char *addr,int n, TMsg* parent);
    TClntOptIAAddress(SmartPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent);
    bool doDuties();
    bool isValid();
};

#endif /* IAADDRESS_H_HEADER_INCLUDED_C112313F */
