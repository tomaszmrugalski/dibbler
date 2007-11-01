/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgLeaseQueryReply.cpp,v 1.1 2007-11-01 08:10:34 thomson Exp $
 */

#include "SrvMsgLeaseQueryReply.h"
#include "Logger.h"
#include "Logger.h"

TSrvMsgLeaseQueryReply::TSrvMsgLeaseQueryReply(SmartPtr<TSrvIfaceMgr> IfaceMgr,
				   SmartPtr<TSrvTransMgr> TransMgr,
				   SmartPtr<TSrvCfgMgr> CfgMgr,
				   SmartPtr<TSrvAddrMgr> AddrMgr,
				   SmartPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,
	     query->getIface(), query->getAddr(), LEASEQUERY_REPLY_MSG,
	     query->getTransID())
{
    if (!answer(query))
	IsDone = true;
    else
	IsDone = false;
}

bool TSrvMsgLeaseQueryReply::answer(SmartPtr<TSrvMsgLeaseQuery> query) {
    Log(Crit) << "#### TODO " << LogEnd;
    return true;
}

bool TSrvMsgLeaseQueryReply::check() {
    // this should never happen
    return true;
}

TSrvMsgLeaseQueryReply::~TSrvMsgLeaseQueryReply() {
}

unsigned long TSrvMsgLeaseQueryReply::getTimeout() {
    return 0;
}
void TSrvMsgLeaseQueryReply::doDuties() {
    IsDone = true;
}

string TSrvMsgLeaseQueryReply::getName() {
    return "LEASE-QUERY-REPLY";
}
