/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRebind.h,v 1.2 2004-06-20 17:25:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

class TSrvMsgRebind;
#ifndef SRVMSGREBIND_H
#define SRVMSGREBIND_H
#include "SrvMsg.h"

class TSrvMsgRebind : public TSrvMsg
{
  public:
    TSrvMsgRebind(SmartPtr<TSrvIfaceMgr>, 
		  SmartPtr<TSrvTransMgr>, 
		  SmartPtr<TSrvCfgMgr>, 
		  SmartPtr<TSrvAddrMgr> AddrMgr,
		  int iface,  SmartPtr<TIPv6Addr> addr);
    
    TSrvMsgRebind(SmartPtr<TSrvIfaceMgr>, 
		  SmartPtr<TSrvTransMgr>, 
		  SmartPtr<TSrvCfgMgr>, 
		  SmartPtr<TSrvAddrMgr> AddrMgr,
		  int iface, SmartPtr<TIPv6Addr> addr,
		  char* buf, int bufSize);
    
    void answer(SmartPtr<TMsg> Rep);
    void doDuties();
    string getName();
    unsigned long getTimeout();
    bool check();
    ~TSrvMsgRebind();
};

#endif /* SRVMSGREBIND_H */
