/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRenew.h,v 1.3 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 *
 */

class TSrvMsgRenew;
#ifndef SRVMSGRENEW_H
#define SRVMSGRENEW_H
#include "SrvMsg.h"

class TSrvMsgRenew : public TSrvMsg
{
 public:
    TSrvMsgRenew(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		 SmartPtr<TSrvTransMgr> TransMgr,
		 SmartPtr<TSrvCfgMgr> CfgMgr,
		 SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, SmartPtr<TIPv6Addr> addr);
    
    TSrvMsgRenew(SmartPtr<TSrvIfaceMgr> IfaceMgr,SmartPtr<TSrvTransMgr> TransMgr,
		 SmartPtr<TSrvCfgMgr> CfgMgr,SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, SmartPtr<TIPv6Addr> addr,
		 char* buf, int bufSize);
    
    void doDuties();
    unsigned long getTimeout();
    string getName();
    bool check();
    ~TSrvMsgRenew();
};

#endif /* SRVMSGRENEW_H */
