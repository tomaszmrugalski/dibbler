#ifndef CLNTOPTIONREQUEST_H_HEADER_INCLUDED_C1123F5A
#define CLNTOPTIONREQUEST_H_HEADER_INCLUDED_C1123F5A

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"
#include "OptClientIdentifier.h"
#include "ClntOptClientIdentifier.h"
#include "OptOptionRequest.h"

class TClntConfMgr;

class TClntOptOptionRequest : public TOptOptionRequest 
{
  public:
    TClntOptOptionRequest(SmartPtr<TClntCfgIface> ptrIface, TMsg* parent);
    TClntOptOptionRequest( char * buf,  int n, TMsg* parent);
    bool doDuties();
 private:
    SmartPtr<TClntCfgMgr> CfgMgr;
};

#endif
