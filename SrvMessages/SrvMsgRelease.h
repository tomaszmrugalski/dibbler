/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRelease.h,v 1.2 2004-06-20 17:25:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

class TSrvMsgRelease;
#ifndef SRVMSGRELEASE_H
#define SRVMSGRELEASE_H

#include "SrvMsg.h"

class TSrvMsgRelease : public TSrvMsg
{
  public:
    TSrvMsgRelease(SmartPtr<TSrvIfaceMgr> IfMgr, 
		SmartPtr<TSrvTransMgr> TransMgr,
		SmartPtr<TSrvCfgMgr>   ConfMgr, 
		SmartPtr<TSrvAddrMgr>  AddrMgr, 
		 int iface, 
		 SmartPtr<TIPv6Addr> addr,
		 char* buf,
		 int bufSize);
	
    void answer(SmartPtr<TMsg> Rep);
    void doDuties();
    unsigned long getTimeout();
    bool check();
    string getName();
    ~TSrvMsgRelease();
};
#endif /* SRVMSGRELEASE_H */
