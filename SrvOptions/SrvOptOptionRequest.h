#ifndef SRVOPTIONREQUEST_H_HEADER_INCLUDED_C1123F5A
#define SRVOPTIONREQUEST_H_HEADER_INCLUDED_C1123F5A

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"
#include "OptClientIdentifier.h"
#include "SrvOptClientIdentifier.h"
#include "OptOptionRequest.h"

class TSrvConfMgr;

class TSrvOptOptionRequest : public TOptOptionRequest 
{
  public:
    TSrvOptOptionRequest(char * buf,  int n, TMsg* parent);
    TSrvOptOptionRequest(TMsg* parent);
    bool doDuties();
};

#endif
