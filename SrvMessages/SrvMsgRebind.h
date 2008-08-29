/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgRebind.h,v 1.5 2008-08-29 00:07:35 thomson Exp $
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
		  int iface, SmartPtr<TIPv6Addr> addr,
		  char* buf, int bufSize);
    
    void doDuties();
    string getName();
    unsigned long getTimeout();
    bool check();
    ~TSrvMsgRebind();
};

#endif /* SRVMSGREBIND_H */
