/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsg.h,v 1.5 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
	
	void answer(SmartPtr<TMsg> answer);

    SmartPtr<TSrvTransMgr>  SrvTransMgr;
    SmartPtr<TSrvAddrMgr>   SrvAddrMgr;
    SmartPtr<TSrvCfgMgr>    SrvCfgMgr;
    SmartPtr<TSrvIfaceMgr>  SrvIfaceMgr;
    
    bool appendRequestedOptions(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, 
        int iface, SmartPtr<TSrvOptOptionRequest> reqOpt);

    // maximum retransmission timeout
    unsigned long MRT;

    unsigned long getTimeout();
    void doDuties();
    void send();
protected:
    unsigned long FirstTimeStamp; // timestamp of first message transmission
    void setAttribs(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		    SmartPtr<TSrvTransMgr> TransMgr, 
		    SmartPtr<TSrvCfgMgr> CfgMgr,
		    SmartPtr<TSrvAddrMgr> AddrMgr);
};

#endif
