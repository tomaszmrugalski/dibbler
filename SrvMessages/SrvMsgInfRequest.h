/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgInfRequest.h,v 1.2 2004-06-20 17:25:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef SRVINFREQUEST_H
#define SRVINFREQUEST_H

#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvAddrMgr.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "IPv6Addr.h"

class TSrvMsgInfRequest : public TSrvMsg
{
  public:
    TSrvMsgInfRequest(SmartPtr<TSrvIfaceMgr> IfMgr, 
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr>   ConfMgr, 
		   SmartPtr<TSrvAddrMgr>  AddrMgr, 
		   int iface);

    TSrvMsgInfRequest(SmartPtr<TSrvIfaceMgr> IfMgr, 
		      SmartPtr<TSrvTransMgr> TransMgr,
		      SmartPtr<TSrvCfgMgr>   ConfMgr, 
		      SmartPtr<TSrvAddrMgr>  AddrMgr, 
		      int iface, SmartPtr<TIPv6Addr> addr, 
		      char* buf, int bufSize);

    void answer(SmartPtr<TMsg> msg);
    void doDuties();
    bool check();
    unsigned long getTimeout();
    string getName();
    ~TSrvMsgInfRequest();
  private:
    SmartPtr<TSrvAddrMgr> AddrMgr;
    TContainer< SmartPtr<TMsg> > BackupSrvLst;
};


#endif /* SRVMSGINFREQUEST_H */
