/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Karol Podolski <podol(at)ds.pg.gda.pl>
 *          Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvMsgLeaseQueryDone.h"
#include "Logger.h"
#include "SrvOptLQ.h"
#include "AddrClient.h"

TSrvMsgLeaseQueryDone::TSrvMsgLeaseQueryDone(SPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsg(query->getIface(), query->getAddr(), LEASEQUERY_DONE_MSG,
	     query->getTransID())
{

}

#if 0
bool TSrvMsgLeaseQueryDone::check() {
    // this should never happen
    return true;
}

TSrvMsgLeaseQueryDone::~TSrvMsgLeaseQueryDone() {
}

unsigned long TSrvMsgLeaseQueryDone::getTimeout() {
    return 0;
}
void TSrvMsgLeaseQueryDone::doDuties() {
    IsDone = true;
}
#endif

string TSrvMsgLeaseQueryDone::getName() {
    return "LEASE-QUERY-DONE";
}
