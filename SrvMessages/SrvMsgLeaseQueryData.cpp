/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Karol Podolski <podol(at)ds.pg.gda.pl>
 *          Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvMsgLeaseQuery.h"
#include "SrvMsgLeaseQueryData.h"
#include "SrvOptServerIdentifier.h"
#include "SrvCfgMgr.h"
#include "Logger.h"

TSrvMsgLeaseQueryData::TSrvMsgLeaseQueryData(SPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsgLeaseQueryReply(query)
{
    // append CLIENT-ID
    SPtr<TOpt> opt;
    opt = query->getOption(OPTION_CLIENTID);
    if (!opt) {
        Log(Error) << "BLQ: query does not have client-id. Malformed." << LogEnd;
        IsDone = true;
        return;
    }
    Options.push_back(opt);

    // append SERVER-ID
    SPtr<TSrvOptServerIdentifier> ptrSrvID;
    ptrSrvID = new TSrvOptServerIdentifier(SrvCfgMgr().getDUID(), this);
    Options.push_back((Ptr*)ptrSrvID);

    // don't do anything else
    // someone will call appendClientData() on us
    // and then call sendTCP()
}

string TSrvMsgLeaseQueryData::getName() {
    return "LEASE-QUERY-DATA";
}
