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
#include "OptAddrLst.h"
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
    void sendRequest(TOptList requestOptions, int iface);
    void sendInfRequest(TOptList requestOptions, int iface);
    void sendRebind(TOptList ptrIA, int iface);
    void sendRelease(List(TAddrIA) iaLst, SPtr<TAddrIA> ta, List(TAddrIA) pdLst);
    bool handleResponse(SPtr<TClntMsg> question, SPtr<TClntMsg> answer);

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

#ifdef MOD_REMOTE_AUTOCONF
    struct TNeighborInfo {
	typedef enum {
	    NeighborInfoState_Added,   // just added (waiting to be sent)
	    NeighborInfoState_Sent,    // sent, awaiting remote reply
	    NeighborInfoState_Received // remote reply received
	} NeighborInfoState;
      SPtr<TIPv6Addr> srvAddr;
      int ifindex;
      int transid;
      SPtr<TDUID> srvDuid;
      SPtr<TClntMsg> reply;
      SPtr<TIPv6Addr> rcvdAddr;
      NeighborInfoState state;
    TNeighborInfo(SPtr<TIPv6Addr> addr)
	: srvAddr(addr), ifindex(0), transid(0), 
	  srvDuid(0), reply(0), rcvdAddr(0), state(NeighborInfoState_Added) { }
    };
    typedef std::list< SPtr<TNeighborInfo> > TNeighborInfoLst;
    TNeighborInfoLst Neighbors;

    SPtr<TNeighborInfo> neighborInfoGet(SPtr<TIPv6Addr> addr);
    SPtr<TNeighborInfo> neighborInfoGet(int transid);

    SPtr<TNeighborInfo> neighborAdd(int ifindex, SPtr<TIPv6Addr> addr);

    bool checkRemoteSolicits();
    bool updateNeighbors(int ifindex, SPtr<TOptAddrLst> neighbors);
    bool sendRemoteSolicit(SPtr<TNeighborInfo> neighbor);
    bool processRemoteReply(SPtr<TClntMsg> reply);
#endif
    
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
    bool openSockets(SPtr<TClntCfgIface> iface);
    bool populateAddrMgr(SPtr<TClntCfgIface> iface);

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

