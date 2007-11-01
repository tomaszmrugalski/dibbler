/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgLeaseQueryReply.h,v 1.1 2007-11-01 08:10:34 thomson Exp $
 *
 */

#ifndef SRVMSGLEASEQUERYREPLY_H
#define SRVMSGLEASEQUERYREPLY_H

#include "SrvMsg.h"
#include "SrvMsgLeaseQuery.h"
class TSrvMsgLeaseQueryReply : public TSrvMsg
{
  public:
    TSrvMsgLeaseQueryReply(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			   SmartPtr<TSrvTransMgr> TransMgr,
			   SmartPtr<TSrvCfgMgr> CfgMgr,
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgLeaseQuery> query);

    bool answer(SmartPtr<TSrvMsgLeaseQuery> query);
    bool check();
    void doDuties();
    unsigned long getTimeout();
    string getName();
    ~TSrvMsgLeaseQueryReply();
};

#endif /* SRVMSGLEASEQUERYREPLY_H */
