/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * chamges Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgRenew.h,v 1.6 2008-08-29 00:07:35 thomson Exp $
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
