/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgConfirm.h,v 1.5 2008-08-29 00:07:34 thomson Exp $
 *
 */

#ifndef SRVMSGCONFIRM_H
#define SRVMSGCONFIRM_H
#include "SrvMsg.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

// Client sends CONFIRM to a server to verify that his addresses
// are still valid. It could happen when:
// 1. client has restated
// 2. client changed link
class TSrvMsgConfirm : public TSrvMsg
{
 public:
    TSrvMsgConfirm(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		   SmartPtr<TSrvTransMgr> TransMgr, 
		   SmartPtr<TSrvCfgMgr> CfgMgr, 
		   SmartPtr<TSrvAddrMgr> AddrMgr,
		   int iface, SmartPtr<TIPv6Addr> addr,
		   char* buf, int bufSize);
    bool check();
    void doDuties();
    unsigned long getTimeout();
    ~TSrvMsgConfirm();
    string getName();

};
#endif /* SRVMSGCONFIRM_H*/
