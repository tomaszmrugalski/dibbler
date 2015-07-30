/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 */

#include "SrvMsgLeaseQueryReply.h"
#include "Logger.h"
#include "SrvOptLQ.h"
#include "OptStatusCode.h" 
#include "OptDUID.h"
#include "SrvOptIAAddress.h"
#include "SrvOptIAPrefix.h"
#include "AddrClient.h"
#include "SrvCfgMgr.h"

using namespace std;

TSrvMsgLeaseQueryReply::TSrvMsgLeaseQueryReply(SPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsg(query->getIface(), query->getRemoteAddr(), LEASEQUERY_REPLY_MSG,
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
		Options.push_back(new TOptStatusCode(STATUSCODE_UNKNOWNQUERYTYPE, "Invalid Query type.", this) );
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
	    Options.push_back(opt);
	    break;
	}

    }
    if (!count) {
	Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY,
                                             "Required LQ_QUERY option missing.", this));
	return true;
    }

    // append SERVERID
    SPtr<TOptDUID> serverID;
    serverID = new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this);
    Options.push_back((Ptr*)serverID);

    // allocate buffer
    this->send();

    return true;
}

bool TSrvMsgLeaseQueryReply::queryByAddress(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    SPtr<TOpt> opt;
    q->firstOption();
    SPtr<TSrvOptIAAddress> addr;
    SPtr<TIPv6Addr> link = q->getLinkAddr();
    
    while ( opt = q->getOption() ) {
	if (opt->getOptType() == OPTION_IAADDR)
	    addr = (Ptr*) opt;
    }
    if (!addr) {
	Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY,
                                             "Required IAADDR suboption missing.", this));
	return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( addr->getAddr() );
    
    if (!cli) {
	Log(Warning) << "LQ: Assignement for client addr=" << addr->getAddr()->getPlain()
                     << " not found." << LogEnd;
	Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED,
                                              "No binding for this address found.", this) );
	return true;
    }
    
    appendClientData(cli);
    return true;
}

bool TSrvMsgLeaseQueryReply::queryByClientID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    SPtr<TOpt> opt;
    SPtr<TOptDUID> duidOpt;
    SPtr<TDUID> duid;
    SPtr<TIPv6Addr> link = q->getLinkAddr();
    
    q->firstOption();
    while ( opt = q->getOption() ) {
	if (opt->getOptType() == OPTION_CLIENTID) {
	    duidOpt = (Ptr*) opt;
	    duid = duidOpt->getDUID();
	}
    }
    if (!duid) {
	Options.push_back( new TOptStatusCode(STATUSCODE_UNSPECFAIL,
                                              "You didn't send your ClientID.", this) );
	return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( duid );
    
    if (!cli) {
	Log(Warning) << "LQ: Assignement for client duid=" << duid->getPlain()
                     << " not found." << LogEnd;
	Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED,
                                              "No binding for this DUID found.", this) );
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

    unsigned long nowTs = (uint32_t) time(NULL);
    unsigned long cliTs = cli->getLastTimestamp();
    unsigned long diff = nowTs - cliTs;

    Log(Debug) << "LQ: modifying the lifetimes (client last seen " << diff << "secs ago)." << LogEnd;

    // add all assigned addresses
    cli->firstIA();
    while ( ia = cli->getIA() ) {
	ia->firstAddr();
	while ( addr=ia->getAddr() ) {
	    unsigned long a = addr->getPref() - diff;
	    unsigned long b = addr->getValid() - diff;
	    cliData->addOption( new TSrvOptIAAddress(addr->get(), a, b, this) );
	}
    }

    // add all assigned prefixes
    cli->firstPD();
    while ( ia = cli->getPD() ) {
	ia->firstPrefix();
	while (prefix = ia->getPrefix()) {
	    cliData->addOption( new TSrvOptIAPrefix( prefix->get(), prefix->getLength(),
                                                     prefix->getPref(),
						     prefix->getValid(), this));
	}
    }

    cliData->addOption(new TOptDUID(OPTION_CLIENTID, cli->getDUID(), this));

    /// @todo: add all temporary addresses

    // add CLT_TIME
    Log(Debug) << "LQ: Adding CLT_TIME option: " << diff << " second(s)." << LogEnd;

    cliData->addOption( new TSrvOptLQClientTime(diff, this));

    Options.push_back((Ptr*)cliData);
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

string TSrvMsgLeaseQueryReply::getName() const {
    return "LEASE-QUERY-REPLY";
}
