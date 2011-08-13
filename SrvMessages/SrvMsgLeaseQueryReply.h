/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgLeaseQueryReply.h,v 1.3 2008-08-29 00:07:35 thomson Exp $
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
    TSrvMsgLeaseQueryReply(SPtr<TSrvMsgLeaseQuery> query);

    bool queryByAddress(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg);
    bool queryByClientID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg);
    bool queryByLinkAddress(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg);
    bool queryByRemoteID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg);
    bool queryByRelayID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg);
    void appendClientData(SPtr<TAddrClient> cli);

    bool answer(SPtr<TSrvMsgLeaseQuery> query);
    bool check();
    void doDuties();
    unsigned long getTimeout();
    string getName();
    ~TSrvMsgLeaseQueryReply();
};

#endif /* SRVMSGLEASEQUERYREPLY_H */
