/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Karol Podolski <podol(at)ds.pg.gda.pl>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef _SRVMSGLEASEQUERYDONE_H
#define	_SRVMSGLEASEQUERYDONE_H

#include "SrvMsg.h"
#include "SrvMsgLeaseQuery.h"
#include "Logger.h"
#include "SrvOptLQ.h"
#include "AddrClient.h"

class TSrvMsgLeaseQueryDone : public TSrvMsg
{
  public:
    TSrvMsgLeaseQueryDone(SPtr<TSrvMsgLeaseQuery> query);
    string getName();
    ~TSrvMsgLeaseQueryDone();
};


#endif	/* _SRVMSGLEASEQUERYDONE_H */
