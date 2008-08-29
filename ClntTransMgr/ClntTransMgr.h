/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntTransMgr.h,v 1.15 2008-08-29 00:07:29 thomson Exp $
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
    void sendRequest(List(TOpt) requestOptions, int iface);
    void sendInfRequest(TContainer< SmartPtr<TOpt> > requestOptions, int iface);
    void sendRebind( TContainer<SmartPtr<TOpt> > ptrIA, int iface);
    void sendRelease(List(TAddrIA) iaLst, SmartPtr<TAddrIA> ta, List(TAddrIA) pdLst);
    void shutdown();
    bool isDone();
    void setContext(SmartPtr<TClntTransMgr> that);

    char * getCtrlAddr();
    int    getCtrlIface();

    // Backup server list management
    void addAdvertise(SPtr<TMsg> advertise); // adds ADVERTISE to the list
    void firstAdvertise();
    SPtr<TMsg> getAdvertise();
    SPtr<TOpt> getAdvertiseDUID(); // returns server DUID of the best advertise on the list
    void sortAdvertise(); // sorts advertise messages
    void delFirstAdvertise(); // deletes first advertise
    int getMaxPreference();
    int getAdvertiseLstCount();
    void printAdvertiseLst();
    
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
    void sortAdvertiseLst();
    void printLst(List(TMsg) lst);
    // managers
    SmartPtr<TClntCfgMgr>   CfgMgr;
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntAddrMgr>  AddrMgr;
    SmartPtr<TClntTransMgr> That;

    List(TClntMsg) Transactions;
    bool IsDone;         // isDone = true - client operation is finished
    bool Shutdown;       // is shutdown in progress?

    bool BindReuse; // Bug #56. Shall we allow running client and server on the same machine?

    int ctrlIface;
    char ctrlAddr[48];

    List(TMsg) AdvertiseLst; // list of backup servers (i.e. not used ADVERTISE messages)
};

#endif

