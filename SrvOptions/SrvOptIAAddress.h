#ifndef SRVIAADDRESS_H_HEADER_INCLUDED_C112313F
#define SRVTIAADDRESS_H_HEADER_INCLUDED_C112313F

#include "SmartPtr.h"
#include "Container.h"
#include "OptIAAddress.h"

class TSrvOptIAAddress : public TOptIAAddress
{
  public:
    TSrvOptIAAddress( char * addr, int n, TMsg* parent);

    TSrvOptIAAddress( SmartPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent);

    bool doDuties();
};

#endif /* IAADDRESS_H_HEADER_INCLUDED_C112313F */
