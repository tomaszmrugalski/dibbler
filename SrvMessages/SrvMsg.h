/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsg.h,v 1.8 2005-02-07 20:51:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
#include "SmartPtr.h"

class TSrvMsg : public TMsg
{
public:
	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		int iface,  SmartPtr<TIPv6Addr> addr, char* buf,  int bufSize);
	
	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		int iface, SmartPtr<TIPv6Addr> addr,  int msgType);

	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID);

	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr);
	
    SmartPtr<TSrvTransMgr>  SrvTransMgr;
    SmartPtr<TSrvAddrMgr>   SrvAddrMgr;
    SmartPtr<TSrvCfgMgr>    SrvCfgMgr;
    SmartPtr<TSrvIfaceMgr>  SrvIfaceMgr;
    
    void copyRelayInfo(SmartPtr<TSrvMsg> q);
    bool appendRequestedOptions(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, 
				int iface, SmartPtr<TSrvOptOptionRequest> reqOpt);
    string showRequestedOptions(SmartPtr<TSrvOptOptionRequest> oro);

    void addRelayInfo(SmartPtr<TIPv6Addr> linkAddr,
		      SmartPtr<TIPv6Addr> peerAddr,
		      int hop,
		      SmartPtr<TSrvOptInterfaceID> interfaceID);

    int getRelayCount();

    virtual bool check() = 0;


    unsigned long getTimeout();
    void doDuties();
    void send();
protected:
    unsigned long FirstTimeStamp; // timestamp of first message transmission
    unsigned long MRT;            // maximum retransmission timeout

    void setAttribs(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		    SmartPtr<TSrvTransMgr> TransMgr, 
		    SmartPtr<TSrvCfgMgr> CfgMgr,
		    SmartPtr<TSrvAddrMgr> AddrMgr);

    SmartPtr<TIPv6Addr> LinkAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TIPv6Addr> PeerAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TSrvOptInterfaceID> InterfaceIDTbl[HOP_COUNT_LIMIT];
    int HopTbl[HOP_COUNT_LIMIT];
    int Relays;
};

#endif
