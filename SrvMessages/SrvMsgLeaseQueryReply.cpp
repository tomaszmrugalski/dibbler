/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgLeaseQueryReply.cpp,v 1.3 2007-12-04 08:57:05 thomson Exp $
 */

#include "SrvMsgLeaseQueryReply.h"
#include "Logger.h"
#include "SrvOptLQ.h"
#include "SrvOptStatusCode.h" 
#include "SrvOptIAAddress.h"
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

    }
    if (!count) {
	Options.append(new TSrvOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required LQ_QUERY option missing.", this));
	return true;
    }

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
	Options.append( new TSrvOptStatusCode(STATUSCODE_NOBINDING, "No binding found.", this) );
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
	Options.append( new TSrvOptStatusCode(STATUSCODE_UNKNOWNQUERYTYPE, "Invalid Query type.", this) );
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

    // add all assigned addresses
    cli->firstIA();
    while ( ia = cli->getIA() ) {
	ia->firstAddr();
	while ( addr=ia->getAddr() ) {
	    cliData->addOption( new TSrvOptIAAddress(addr->get(), addr->getPref(), addr->getValid(), this) );
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
