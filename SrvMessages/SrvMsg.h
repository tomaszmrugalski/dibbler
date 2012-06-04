/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVMSG_H
#define SRVMSG_H

#include "Msg.h"
#include "SmartPtr.h"
#include "SrvAddrMgr.h"
#include "IPv6Addr.h"
#include "SrvCfgIface.h"
#include "OptOptionRequest.h"
#include "SrvOptInterfaceID.h"
#include "SrvOptFQDN.h"
#include "SrvOptIA_NA.h"
#include "SrvOptTA.h"
#include "SrvOptIA_PD.h"
#include "OptVendorData.h"
#include "OptGeneric.h"

class TSrvMsg : public TMsg
{
public:
    TSrvMsg(int iface,  SPtr<TIPv6Addr> addr, char* buf,  int bufSize);
    TSrvMsg(int iface, SPtr<TIPv6Addr> addr, int msgType, long transID);

    void copyRelayInfo(SPtr<TSrvMsg> q);
    void copyAAASPI(SPtr<TSrvMsg> q);
    void copyRemoteID(SPtr<TSrvMsg> q);
    bool copyClientID(SPtr<TMsg> donor);

    void appendAuthenticationOption(SPtr<TDUID> duid);
    bool appendMandatoryOptions(SPtr<TOptOptionRequest> oro, bool includeClientID = true);
    bool appendRequestedOptions(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr,
                                int iface, SPtr<TOptOptionRequest> reqOpt);
    std::string showRequestedOptions(SPtr<TOptOptionRequest> oro);
    bool appendVendorSpec(SPtr<TDUID> duid, int iface, int vendor, SPtr<TOptOptionRequest> reqOpt);
    void appendStatusCode();

    void addRelayInfo(SPtr<TIPv6Addr> linkAddr,
                      SPtr<TIPv6Addr> peerAddr,
                      int hop,
                      SPtr<TSrvOptInterfaceID> interfaceID,
                      List(TOptGeneric) echoList);

    int getRelayCount();

    bool releaseAll(bool quiet);

    bool validateReplayDetection();

    virtual bool check() = 0;

    void setRemoteID(SPtr<TOptVendorData> remoteID);
    SPtr<TOptVendorData> getRemoteID();

    unsigned long getTimeout();
    void doDuties();
    void send();

    void processOptions(SPtr<TSrvMsg> clientMsg, bool quiet);
    SPtr<TDUID> getClientDUID();
                SPtr<TIPv6Addr> getClientPeer();

protected:
    SPtr<TOptOptionRequest> ORO;
    void handleDefaultOption(SPtr<TOpt> ptrOpt);
    void getORO(SPtr<TMsg> clientMessage);
    SPtr<TDUID> ClientDUID;

    bool check(bool clntIDmandatory, bool srvIDmandatory);

    void processIA_NA(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_NA> q);
    void processIA_TA(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptTA> q);
    void processIA_PD(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> q);
    void processFQDN(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptFQDN> q);

#if 0
    SPtr<TSrvOptFQDN> prepareFQDN(SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid,
                                  SPtr<TIPv6Addr> clntAddr, std::string hint, bool doRealUpdate);
    void fqdnRelease(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ia, SPtr<TFQDN> fqdn);
#endif
    SPtr<TSrvOptFQDN> addFQDN(int iface, SPtr<TSrvOptFQDN> requestFQDN,
                              SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,
                              std::string hint, bool doRealUpdate);
    void delFQDN(SPtr<TSrvCfgIface> cfgIface, SPtr<TAddrIA> ptrIA, SPtr<TFQDN> fqdn);


    int storeSelfRelay(char * buf, int relayLevel, ESrvIfaceIdOrder order);


    // relay
    SPtr<TIPv6Addr> LinkAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TIPv6Addr> PeerAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TSrvOptInterfaceID> InterfaceIDTbl[HOP_COUNT_LIMIT];
    List(TOptGeneric) EchoListTbl[HOP_COUNT_LIMIT];  // list of options to be echoed back
    int len[HOP_COUNT_LIMIT];
    int HopTbl[HOP_COUNT_LIMIT];

    unsigned long FirstTimeStamp; // timestamp of first message transmission
    unsigned long MRT;            // maximum retransmission timeout
    int Relays;

    SPtr<TOptVendorData> RemoteID; // this MAY be set, if message was recevied via relay AND relay appended this RemoteID
    int Parent; // type of the parent message (used in ADVERTISE and REPLY)

};

#endif
