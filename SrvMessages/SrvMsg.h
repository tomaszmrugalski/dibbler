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

#include <vector>
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
    struct RelayInfo {
        int Hop_;
        SPtr<TIPv6Addr> LinkAddr_;
        SPtr<TIPv6Addr> PeerAddr_;
        size_t Len_;
        TOptList EchoList_;
    };

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

#ifndef MOD_DISABLE_AUTH
    void appendReconfigureKey();
    bool validateReplayDetection();
#endif

    /// @todo: modify this to use RelayInfo structure
    void addRelayInfo(SPtr<TIPv6Addr> linkAddr,
                      SPtr<TIPv6Addr> peerAddr,
                      int hop,
                      const TOptList& echoList);

    bool releaseAll(bool quiet);


    virtual bool check() = 0;

    /// @todo: get out with this shit
    void setRemoteID(SPtr<TOptVendorData> remoteID);
    SPtr<TOptVendorData> getRemoteID();

    unsigned long getTimeout();
    void doDuties();
    void send(int dstPort = 0);

    void processOptions(SPtr<TSrvMsg> clientMsg, bool quiet);
    SPtr<TDUID> getClientDUID();
    SPtr<TIPv6Addr> getClientPeer();

    /// @brief sets message type (used in testing only)
    ///
    /// @param type type of a message (RELAY_FORW or RELAY_REPL)
    inline void setMsgType(uint8_t type) {
      forceMsgType_ = type;
    }

    /// @brief clears relay information (used in testing only)
    void clearRelayInfo() {
        RelayInfo_.clear();
    }

    /// @brief Relay information
    ///
    /// Let's make this public, so there are issues with reference
    /// being returned and then used past them message lifetime
    std::vector<RelayInfo> RelayInfo_;

    void setPhysicalIface(int iface);
    int  getPhysicalIface() const;


protected:
    void setDefaults();
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


    int storeSelfRelay(char * buf, uint8_t relayLevel, ESrvIfaceIdOrder order);

    unsigned long FirstTimeStamp_; // timestamp of first message transmission
    unsigned long MRT_;            // maximum retransmission timeout

    /// @todo: this should be moved to RelayInfo_ structure
    SPtr<TOptVendorData> RemoteID; // this MAY be set, if message was recevied via relay
                                   // AND relay appended RemoteID

    /// used in tests only. If non-zero, send message type is set to that type
    uint8_t forceMsgType_;

    /// physical interface from/to which message was received/should be sent
    int physicalIface_;
};

typedef std::vector< SPtr<TSrvMsg> > SrvMsgList;

#endif
