class TClntTransMgr;
#ifndef CLNTTRANSMGR_H
#define CLNTTRANSMGR_H
#include <string>
#include "ClntIfaceMgr.h"
#include "Opt.h"
#include "IPv6Addr.h"
class TDHCPMsg;
class TClntAddrMgr;
class TMsg;
class TClntConfMgr;

class TClntTransMgr
{
  public:
    TClntTransMgr(SmartPtr<TClntIfaceMgr> ifaceMgr, 
		  string config);
    void doDuties();
    void relayMsg(SmartPtr<TMsg> msg);
    unsigned long getTimeout();
    void stop();
    void sendRequest(TContainer< SmartPtr<TOpt> > requestOptions, 
		     TContainer< SmartPtr<TMsg> > srvlist,int iface);
    void sendInfRequest(TContainer< SmartPtr<TOpt> > requestOptions, int iface);
    void sendRebind( TContainer<SmartPtr<TOpt> > ptrIA, int iface);
    void sendRelease(List(TAddrIA) ptrIA);
    void shutdown();
    bool isDone();
    void setThat(SmartPtr<TClntTransMgr> that);

	char * getCtrlAddr();
	int    getCtrlIface();
    
 protected:
    void removeExpired();
    void checkDecline();
    void checkConfirm();
    void checkDB();
    void checkRenew();
    void checkRequest();
    void checkSolicit();
    void checkInfRequest();
  private:
    // managers
    SmartPtr<TClntCfgMgr>   CfgMgr;
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntAddrMgr>  AddrMgr;
    SmartPtr<TClntTransMgr> That;

    TContainer< SmartPtr<TMsg> > Transactions;

    bool IsDone;
    bool Shutdown;
    bool isStart;

	int ctrlIface;
	char ctrlAddr[48];
};
#endif

