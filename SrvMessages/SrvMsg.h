/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsg.h,v 1.12 2006-10-06 00:42:58 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2006-08-30 01:10:39  thomson
 * DNSUpdate general cleanup.
 *
 * Revision 1.10  2006-08-24 01:12:29  thomson
 * FQDN hint implementation by Krzysiek Wnuk, fixes by Thomson.
 *
 * Revision 1.9  2006-08-21 21:33:20  thomson
 * prepareFQDN() moved from SrvMgrReply to SrvMsg,
 * unnecessary constructors removed.
 *
 * Revision 1.8  2005-02-07 20:51:56  thomson
 * Server stateless mode fixed (bug #103)
 *
 * Revision 1.7  2005/01/12 00:10:05  thomson
 * Compilation fixes.
 *
 * Revision 1.6  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.5  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.4  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
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
    unsigned long FirstTimeStamp; // timestamp of first message transmission
    unsigned long MRT;            // maximum retransmission timeout

    void setAttribs(SPtr<TSrvIfaceMgr> IfaceMgr, 
		    SPtr<TSrvTransMgr> TransMgr, 
		    SPtr<TSrvCfgMgr> CfgMgr,
		    SPtr<TSrvAddrMgr> AddrMgr);

    SPtr<TSrvOptFQDN> prepareFQDN(SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
				  SPtr<TIPv6Addr> clntAddr, string hint, bool doRealUpdate);
    void fqdnRelease(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ia, SPtr<TFQDN> fqdn);

    SPtr<TIPv6Addr> LinkAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TIPv6Addr> PeerAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TSrvOptInterfaceID> InterfaceIDTbl[HOP_COUNT_LIMIT];
    int HopTbl[HOP_COUNT_LIMIT];
    int Relays;
    int Parent; // type of the parent message (used in ADVERTISE and REPLY)
};

#endif
