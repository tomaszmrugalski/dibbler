/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Grzegorz Pluto
 *
 * released under GNU GPL v2 only licence
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
    static void instanceCreate(const std::string& config, int port);
    static TSrvTransMgr &instance();

    bool openSocket(SPtr<TSrvCfgIface> confIface, int port);

    long getTimeout();
    void relayMsg(SPtr<TSrvMsg> msg);

    /// @brief Checks whether message was sent to unicast when it was forbidden
    ///
    /// Client is allowed to send data to unicast only if the server is
    /// configured with server-unicast option. Otherwise it will send
    /// back message with only status-code=UseMulticast, client-id and server-id
    /// @return true (accept message) or false (drop it)
    bool unicastCheck(SPtr<TSrvMsg> msg);

    void doDuties();
    void dump();

    bool isDone();
    void shutdown();

#if 0
    SPtr<TSrvOptFQDN> addFQDN(int iface, SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
                              SPtr<TIPv6Addr> clntAddr, std::string hint, bool doRealUpdate);
    void removeFQDN(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ptrIA, SPtr<TFQDN> fqdn);
#endif

    bool sanitizeAddrDB();

    void removeExpired(std::vector<TSrvAddrMgr::TExpiredInfo>& addrLst,
                       std::vector<TSrvAddrMgr::TExpiredInfo>& tempAddrLst,
                       std::vector<TSrvAddrMgr::TExpiredInfo>& prefixLst);

    void notifyExpireInfo(TNotifyScriptParams& params, const TSrvAddrMgr::TExpiredInfo& exp,
                          TIAType type);

    char * getCtrlAddr();
    int    getCtrlIface();

    int checkReconfigures();

    bool sendReconfigure(SPtr<TIPv6Addr> addr, int iface,
                         int msgType, SPtr<TDUID> ptrDUID);

    bool ClientInPool1(SPtr<TIPv6Addr> addr, int iface,bool PD);

    /// @brief sends specified packet
    ///
    /// @param msg message to be sent
    virtual void sendPacket(SPtr<TSrvMsg> msg);

    // not private, as we need to instantiate derived SrvTransMgr in tests
  protected:
    TSrvTransMgr(std::string xmlFile, int port);
    virtual ~TSrvTransMgr();

    std::string XmlFile;
    bool IsDone;

    int ctrlIface;
    char ctrlAddr[48]; // @todo: WTF is that? It should be TIPv6Addr

    static TSrvTransMgr * Instance;

    int port_;
};



#endif
