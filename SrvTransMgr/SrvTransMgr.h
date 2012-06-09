/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvTransMgr.h,v 1.12 2008-10-12 20:10:25 thomson Exp $
 *
 */

#ifndef SRVTRANSMGR_H
#define SRVTRANSMGR_H

#include <string>
#include <vector>
#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "SrvMsg.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgIface.h"
#include "SrvAddrMgr.h"

#define SrvTransMgr() (TSrvTransMgr::instance())

class TSrvTransMgr
{
    friend std::ostream & operator<<(std::ostream &strum, TSrvTransMgr &x);
  public:
    static void instanceCreate(const std::string config);
    static TSrvTransMgr &instance();

    bool openSocket(SPtr<TSrvCfgIface> confIface);

    long getTimeout();
    void relayMsg(SPtr<TSrvMsg> msg);
    void doDuties();
    void dump();

    bool isDone();
    void shutdown();

#if 0
    SPtr<TSrvOptFQDN> addFQDN(int iface, SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
                              SPtr<TIPv6Addr> clntAddr, std::string hint, bool doRealUpdate);
    void removeFQDN(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ptrIA, SPtr<TFQDN> fqdn);
#endif

    void removeExpired(std::vector<TSrvAddrMgr::TExpiredInfo>& addrLst,
                       std::vector<TSrvAddrMgr::TExpiredInfo>& tempAddrLst,
                       std::vector<TSrvAddrMgr::TExpiredInfo>& prefixLst);

    void notifyExpireInfo(TNotifyScriptParams& params, const TSrvAddrMgr::TExpiredInfo& exp,
                          TAddrIA::TIAType type);

    char * getCtrlAddr();
    int    getCtrlIface();

    // not private, as we need to instantiate derived SrvTransMgr in tests
  protected:
    TSrvTransMgr(std::string xmlFile);
    ~TSrvTransMgr();

    std::string XmlFile;
    List(TSrvMsg) MsgLst;
    bool IsDone;

    int ctrlIface;
    char ctrlAddr[48]; // @todo: WTF is that? It should be TIPv6Addr

    //SPtr<TSrvMsg> requestMsg; /// @todo: Remove this field and do the REQUEST handling properly

    static TSrvTransMgr * Instance;
};



#endif


