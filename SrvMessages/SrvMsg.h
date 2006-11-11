/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsg.h,v 1.13 2006-11-11 06:56:27 thomson Exp $
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
    bool appendRequestedOptions(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr, 
				int iface, SPtr<TSrvOptOptionRequest> reqOpt);
    string showRequestedOptions(SPtr<TSrvOptOptionRequest> oro);

    void addRelayInfo(SPtr<TIPv6Addr> linkAddr,
		      SPtr<TIPv6Addr> peerAddr,
		      int hop,
		      SPtr<TSrvOptInterfaceID> interfaceID);

    int getRelayCount();

    virtual bool check() = 0;

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
    
    SPtr<TIPv6Addr> LinkAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TIPv6Addr> PeerAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TSrvOptInterfaceID> InterfaceIDTbl[HOP_COUNT_LIMIT];
    unsigned long FirstTimeStamp; // timestamp of first message transmission
    unsigned long MRT;            // maximum retransmission timeout
    int HopTbl[HOP_COUNT_LIMIT];
    int Relays;
    int Parent; // type of the parent message (used in ADVERTISE and REPLY)
};

#endif
