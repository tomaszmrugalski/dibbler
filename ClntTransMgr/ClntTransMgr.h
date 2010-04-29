/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTTRANSMGR_H
#define CLNTTRANSMGR_H
#include <string>
#include "ClntCfgIface.h"
#include "Opt.h"
#include "IPv6Addr.h"
#include "AddrIA.h"
#include "ClntMsg.h"

#define ClntTransMgr() (TClntTransMgr::instance())

class TClntTransMgr
{
 private:
  TClntTransMgr(const std::string config);

  public:
    static void instanceCreate(const std::string config);
    static TClntTransMgr &instance();
    ~TClntTransMgr();
    void doDuties();
    void relayMsg(SPtr<TClntMsg> msg);
    unsigned long getTimeout();
    void stop();
    void sendRequest(List(TOpt) requestOptions, int iface);
    void sendInfRequest(TContainer< SPtr<TOpt> > requestOptions, int iface);
    void sendRebind( TContainer<SPtr<TOpt> > ptrIA, int iface);
    void sendRelease(List(TAddrIA) iaLst, SPtr<TAddrIA> ta, List(TAddrIA) pdLst);
    void shutdown();
    bool isDone();

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
    bool openSocket(SPtr<TClntCfgIface> iface);
    void sortAdvertiseLst();
    void printLst(List(TMsg) lst);

    List(TClntMsg) Transactions;
    bool IsDone;         // isDone = true - client operation is finished
    bool Shutdown;       // is shutdown in progress?

    bool BindReuse; // Bug #56. Shall we allow running client and server on the same machine?

    int ctrlIface;
    char ctrlAddr[48];

    List(TMsg) AdvertiseLst; // list of backup servers (i.e. not used ADVERTISE messages)

    static TClntTransMgr * Instance;
};

#endif

