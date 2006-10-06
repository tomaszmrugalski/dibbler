/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * chamges Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRenew.h,v 1.5 2006-10-06 00:42:58 thomson Exp $
 *
 */

class TSrvMsgRenew;
#ifndef SRVMSGRENEW_H
#define SRVMSGRENEW_H
#include "SrvMsg.h"

class TSrvMsgRenew : public TSrvMsg
{
 public:
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
