/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgLeaseQueryReply.cpp,v 1.5 2007-12-09 04:10:06 thomson Exp $
 */

#include "SrvMsgLeaseQueryReply.h"
#include "Logger.h"
#include "SrvOptLQ.h"
#include "SrvOptStatusCode.h" 
#include "SrvOptIAAddress.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIAPrefix.h"
#include "SrvOptClientIdentifier.h"
#include "AddrClient.h"


TSrvMsgLeaseQueryReply::TSrvMsgLeaseQueryReply(SmartPtr<TSrvIfaceMgr> IfaceMgr,
				   SmartPtr<TSrvTransMgr> TransMgr,
				   SmartPtr<TSrvCfgMgr> CfgMgr,
				   SmartPtr<TSrvAddrMgr> AddrMgr,
				   SmartPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,
	     query->getIface(), query->getAddr(), LEASEQUERY_REPLY_MSG,
	     query->getTransID())
{
  if (!answer(query)) {
    Log(Error) << "LQ: LQ-QUERY response generation failed." << LogEnd;
	IsDone = true;
  } else {
    Log(Debug) << "LQ: LQ-QUERY response generation successful." << LogEnd;
    IsDone = false;
  }
}


/** 
 * 
 * 
 * @param queryMsg 
 * 
 * @return true - answer should be sent
 */
bool TSrvMsgLeaseQueryReply::answer(SPtr<TSrvMsgLeaseQuery> queryMsg) {

    int count = 0;
    SPtr<TOpt> opt;
    bool ok = true;

    Log(Info) << "LQ: Generating new LEASEQUERY_RESP message." << LogEnd;
    
    queryMsg->firstOption();
    while ( opt = queryMsg->getOption()) {
	switch (opt->getOptType()) {
	case OPTION_LQ_QUERY:
	{
	    count++;
	    SPtr<TSrvOptLQ> q = (Ptr*) opt;
	    switch (q->getQueryType()) {
	    case QUERY_BY_ADDRESS:
		ok = queryByAddress(q, queryMsg);
		break;
	    case QUERY_BY_CLIENTID:
		ok = queryByClientID(q, queryMsg);
		break;
	    default:
		Options.append( new TSrvOptStatusCode(STATUSCODE_UNKNOWNQUERYTYPE, "Invalid Query type.", this) );
		Log(Warning) << "LQ: Invalid query type (" << q->getQueryType() << " received." << LogEnd;
		return true;
	    }
	    if (!ok) {
		Log(Warning) << "LQ: Malformed query detected." << LogEnd;
		return false;
	    }
	    break;
	}
	case OPTION_CLIENTID:
	    // copy the client-id option
	    Options.append(opt);
	    break;
	}

    }
    if (!count) {
	Options.append(new TSrvOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required LQ_QUERY option missing.", this));
	return true;
    }

    // append SERVERID
    SmartPtr<TSrvOptServerIdentifier> ptrSrvID;
    ptrSrvID = new TSrvOptServerIdentifier(SrvCfgMgr->getDUID(),this);
    Options.append((Ptr*)ptrSrvID);

    // allocate buffer
    pkt = new char[getSize()];
    this->send();

    return true;
}

bool TSrvMsgLeaseQueryReply::queryByAddress(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    SPtr<TOpt> opt;
    q->firstOption();
    SPtr<TSrvOptIAAddress> addr = 0;
    SPtr<TIPv6Addr> link = q->getLinkAddr();
    
    while ( opt = q->getOption() ) {
	if (opt->getOptType() == OPTION_IAADDR)
	    addr = (Ptr*) opt;
    }
    if (!addr) {
	Options.append(new TSrvOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required IAADDR suboption missing.", this));
	return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr->getClient( addr->getAddr() );
    
    if (!cli) {
	Log(Warning) << "LQ: Assignement for client addr=" << addr->getAddr()->getPlain() << " not found." << LogEnd;
	Options.append( new TSrvOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this address found.", this) );
	return true;
    }
    
    appendClientData(cli);
    return true;
}

bool TSrvMsgLeaseQueryReply::queryByClientID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    SPtr<TOpt> opt;
    q->firstOption();
    SPtr<TSrvOptClientIdentifier> duidOpt = 0;
    SPtr<TDUID> duid = 0;
    SPtr<TIPv6Addr> link = q->getLinkAddr();
    
    while ( opt = q->getOption() ) {
	if (opt->getOptType() == OPTION_CLIENTID) {
	    duidOpt = (Ptr*) opt;
	    duid = duidOpt->getDUID();
	}
    }
    if (!duid) {
	Options.append( new TSrvOptStatusCode(STATUSCODE_NOBINDING, "No binding found.", this) );
	return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr->getClient( duid );
    
    if (!cli) {
	Log(Warning) << "LQ: Assignement for client duid=" << duid->getPlain() << " not found." << LogEnd;
	Options.append( new TSrvOptStatusCode(STATUSCODE_NOBINDING, "No binding for this DUID found.", this) );
	return true;
    }
    
    appendClientData(cli);
    return true;
}

void TSrvMsgLeaseQueryReply::appendClientData(SPtr<TAddrClient> cli) {

    Log(Debug) << "LQ: Appending data for client " << cli->getDUID()->getPlain() << LogEnd;

    SPtr<TSrvOptLQClientData> cliData = new TSrvOptLQClientData(this);
    
    SPtr<TAddrIA> ia;
    SPtr<TAddrAddr> addr;
    SPtr<TAddrPrefix> prefix;

    unsigned long nowTs = now();
    unsigned long cliTs = cli->getLastTimestamp();
    unsigned long diff = nowTs - cliTs;

    Log(Debug) << "LQ: modifying the lifetimes (client last seen " << diff << "secs ago)." << LogEnd;

    // add all assigned addresses
    cli->firstIA();
    while ( ia = cli->getIA() ) {
	ia->firstAddr();
	while ( addr=ia->getAddr() ) {
	    unsigned long a = addr->getPref();
	    if (a>diff)
		a = 0;
	    unsigned long b = addr->getValid();
	    if (b>diff)
		b = 0;
	    cliData->addOption( new TSrvOptIAAddress(addr->get(), a, b, this) );
	}
    }

    // add all assigned prefixes
    cli->firstPD();
    while ( ia = cli->getPD() ) {
	ia->firstPrefix();
	while (prefix = ia->getPrefix()) {
	    cliData->addOption( new TSrvOptIAPrefix( prefix->getPrefix(), prefix->getLength(), prefix->getPref(),
						     prefix->getValid(), this));
	}
    }

    cliData->addOption(new TSrvOptClientIdentifier(cli->getDUID(), this));

    // TODO: add all temporary addresses

    // add CLT_TIME
    unsigned int ts = now();
    unsigned int clntTs = cli->getLastTimestamp();

    Log(Debug) << "LQ: Adding CLT_TIME option: client timestamp=" << clntTs << ", now=" << ts << ", sending " << (ts - clntTs) << LogEnd;
    ts = ts - clntTs;

    cliData->addOption( new TSrvOptLQClientTime(ts, this));

    Options.append((Ptr*)cliData);
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
