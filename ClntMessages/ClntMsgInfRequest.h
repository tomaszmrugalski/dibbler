#ifndef INFREQUEST_H_HEADER_INCLUDED_C1127952
#define INFREQUEST_H_HEADER_INCLUDED_C1127952

#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgMgr.h"

class TClntIfaceMgr;


class TClntMsgInfRequest : public TClntMsg
{
  public:

      TClntMsgInfRequest(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   ConfMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
		    TContainer< SmartPtr<TOpt> > ReqOpts,
		    int iface);
    
      TClntMsgInfRequest(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   ConfMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
            SmartPtr<TClntCfgIface> iface);

    void answer(SmartPtr<TMsg> msg);
    void doDuties();    
    bool check();
    ~TClntMsgInfRequest();

  private:
    SmartPtr<TClntAddrMgr> AddrMgr;
    
};



#endif 
