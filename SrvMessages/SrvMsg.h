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
 * $Id: SrvMsg.h,v 1.20 2008-08-29 00:07:34 thomson Exp $
 *
 */

class TSrvMsg;
#ifndef SRVMSG_H
#define SRVMSG_H

#include "Msg.h"
#include "SmartPtr.h"
#include "Opt.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"
#include "IPv6Addr.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptInterfaceID.h"
#include "SrvOptFQDN.h"
#include "SrvOptRemoteID.h"
#include "SrvOptGeneric.h"

class TSrvMsg : public TMsg
{
public:
    TSrvMsg(SPtr<TSrvIfaceMgr> IfaceMgr, SPtr<TSrvTransMgr> TransMgr, 
	    SPtr<TSrvCfgMgr> CfgMgr,     SPtr<TSrvAddrMgr> AddrMgr,
	    int iface,  SPtr<TIPv6Addr> addr, char* buf,  int bufSize);
    
    TSrvMsg(SPtr<TSrvIfaceMgr> IfaceMgr, SPtr<TSrvTransMgr> TransMgr, 
	    SPtr<TSrvCfgMgr> CfgMgr,     SPtr<TSrvAddrMgr> AddrMgr,
	    int iface, SPtr<TIPv6Addr> addr, int msgType, long transID);
    
    TSrvMsg(SPtr<TSrvIfaceMgr> IfaceMgr, SPtr<TSrvTransMgr> TransMgr, 
	    SPtr<TSrvCfgMgr> CfgMgr,     SPtr<TSrvAddrMgr> AddrMgr);
    
    SPtr<TSrvTransMgr>  SrvTransMgr;
    SPtr<TSrvAddrMgr>   SrvAddrMgr;
    SPtr<TSrvCfgMgr>    SrvCfgMgr;
    SPtr<TSrvIfaceMgr>  SrvIfaceMgr;
    
    void copyRelayInfo(SPtr<TSrvMsg> q);
    void copyAAASPI(SPtr<TSrvMsg> q);
    void copyRemoteID(SPtr<TSrvMsg> q);

    void appendAuthenticationOption(SmartPtr<TDUID> duid);

    bool appendRequestedOptions(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr, 
				int iface, SPtr<TSrvOptOptionRequest> reqOpt);
    string showRequestedOptions(SPtr<TSrvOptOptionRequest> oro);
    bool appendVendorSpec(SPtr<TDUID> duid, int iface, int vendor, SPtr<TSrvOptOptionRequest> reqOpt);
    void appendStatusCode();
    bool delOption(int code);

    void addRelayInfo(SPtr<TIPv6Addr> linkAddr,
		      SPtr<TIPv6Addr> peerAddr,
		      int hop,
		      SPtr<TSrvOptInterfaceID> interfaceID,
		      List(TSrvOptGeneric) echoList);

    int getRelayCount();

    bool validateReplayDetection();

    SmartPtr<TSrvTransMgr> getSrvTransMgr();

    virtual bool check() = 0;

    void setRemoteID(SPtr<TSrvOptRemoteID> remoteID);
    SPtr<TSrvOptRemoteID> getRemoteID();

    unsigned long getTimeout();
    void doDuties();
    void send();
protected:
    void setAttribs(SPtr<TSrvIfaceMgr> IfaceMgr, 
		    SPtr<TSrvTransMgr> TransMgr, 
		    SPtr<TSrvCfgMgr> CfgMgr,
		    SPtr<TSrvAddrMgr> AddrMgr);
    bool check(bool clntIDmandatory, bool srvIDmandatory);
    SPtr<TSrvOptFQDN> prepareFQDN(SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
				  SPtr<TIPv6Addr> clntAddr, string hint, bool doRealUpdate);
    void fqdnRelease(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ia, SPtr<TFQDN> fqdn);
    int storeSelfRelay(char * buf, int relayLevel, ESrvIfaceIdOrder order);

    // relay
    SPtr<TIPv6Addr> LinkAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TIPv6Addr> PeerAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TSrvOptInterfaceID> InterfaceIDTbl[HOP_COUNT_LIMIT];
    List(TSrvOptGeneric) EchoListTbl[HOP_COUNT_LIMIT];  // list of options to be
    int len[HOP_COUNT_LIMIT];
    int HopTbl[HOP_COUNT_LIMIT];

    unsigned long FirstTimeStamp; // timestamp of first message transmission
    unsigned long MRT;            // maximum retransmission timeout
    int Relays;

    SPtr<TSrvOptRemoteID> RemoteID; // this MAY be set, if message was recevied via relay AND relay appended this RemoteID
    int Parent; // type of the parent message (used in ADVERTISE and REPLY)
};

#endif
