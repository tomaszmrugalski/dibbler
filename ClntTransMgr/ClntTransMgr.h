/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntTransMgr.h,v 1.10 2006-07-16 11:38:24 thomson Exp $
 *
 */
class TClntTransMgr;
class TDHCPMsg;
class TClntAddrMgr;
class TMsg;
class TClntMsg;
class TClntConfMgr;

#ifndef CLNTTRANSMGR_H
#define CLNTTRANSMGR_H
#include <string>
#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "Opt.h"
#include "IPv6Addr.h"

class TClntTransMgr
{
  public:
    TClntTransMgr(SmartPtr<TClntIfaceMgr> ifaceMgr, 
		  SmartPtr<TClntAddrMgr> addrMgr,
		  SmartPtr<TClntCfgMgr> cfgMgr,
		  string config);
    ~TClntTransMgr();
    void doDuties();
    void relayMsg(SmartPtr<TClntMsg> msg);
    unsigned long getTimeout();
    void stop();
    void sendRequest(TContainer< SmartPtr<TOpt> > requestOptions, 
		     TContainer< SmartPtr<TMsg> > srvlist,int iface);
    void sendInfRequest(TContainer< SmartPtr<TOpt> > requestOptions, int iface);
    void sendRebind( TContainer<SmartPtr<TOpt> > ptrIA, int iface);
    void sendRelease(List(TAddrIA) iaLst, SmartPtr<TAddrIA> ta);
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
    bool openLoopbackSocket();
    bool openSocket(SmartPtr<TClntCfgIface> iface);

    // managers
    SmartPtr<TClntCfgMgr>   CfgMgr;
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntAddrMgr>  AddrMgr;
    SmartPtr<TClntTransMgr> That;

    List(TClntMsg) Transactions;
    bool IsDone;
    bool Shutdown;
    bool ConfirmEnabled;  // should we send CONFIRM message?

    bool BindReuse; // Bug #56. Shall we allow running client and server on the same machine?

    int ctrlIface;
    char ctrlAddr[48];
};

/* false - normal operation
   true - Linux: client and server can be run on the same host, but
   there is also a drawback: multiple clients can be run at once. */

#endif

