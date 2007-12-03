/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgLeaseQueryReply.h,v 1.2 2007-12-03 16:58:06 thomson Exp $
 *
 */

#ifndef SRVMSGLEASEQUERYREPLY_H
#define SRVMSGLEASEQUERYREPLY_H

#include "SrvMsg.h"
#include "SrvMsgLeaseQuery.h"
#include "Logger.h"
#include "SrvOptLQ.h"
#include "AddrClient.h"

class TSrvMsgLeaseQueryReply : public TSrvMsg
{
  public:
    TSrvMsgLeaseQueryReply(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			   SmartPtr<TSrvTransMgr> TransMgr,
			   SmartPtr<TSrvCfgMgr> CfgMgr,
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgLeaseQuery> query);

    bool queryByAddress(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg);
    bool queryByClientID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg);
    void appendClientData(SPtr<TAddrClient> cli);

    bool answer(SmartPtr<TSrvMsgLeaseQuery> query);
    bool check();
    void doDuties();
    unsigned long getTimeout();
    string getName();
    ~TSrvMsgLeaseQueryReply();
};

#endif /* SRVMSGLEASEQUERYREPLY_H */
