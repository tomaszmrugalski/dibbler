/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRebind.h,v 1.3 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
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
    
    void doDuties();
    string getName();
    unsigned long getTimeout();
    bool check();
    ~TSrvMsgRebind();
};

#endif /* SRVMSGREBIND_H */
