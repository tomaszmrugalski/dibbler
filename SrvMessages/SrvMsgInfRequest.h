/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgInfRequest.h,v 1.5 2008-08-29 00:07:35 thomson Exp $
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
    TSrvMsgInfRequest(SPtr<TSrvIfaceMgr> IfMgr, 
		      SPtr<TSrvTransMgr> TransMgr,
		      SPtr<TSrvCfgMgr>   ConfMgr, 
		      SPtr<TSrvAddrMgr>  AddrMgr, 
		      int iface, SPtr<TIPv6Addr> addr, 
		      char* buf, int bufSize);
    void doDuties();
    bool check();
    unsigned long getTimeout();
    string getName();
    ~TSrvMsgInfRequest();
  private:
    SPtr<TSrvAddrMgr> AddrMgr;
    List(TMsg) BackupSrvLst;
};


#endif /* SRVMSGINFREQUEST_H */
