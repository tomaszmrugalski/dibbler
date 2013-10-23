/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Damian Manelski <dm1988air@gmail.com>
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
#include "OptRemoteId.h"

using namespace std;

// was query->getAddr()
TSrvMsgLeaseQueryReply::TSrvMsgLeaseQueryReply(SPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsg(query->getIface(), query->getRemoteAddr(), LEASEQUERY_REPLY_MSG,
	     query->getTransID())
{
  if(!query->Bulk) {
      if (!answer(query)) {
        Log(Error) << "LQ: LQ-QUERY response generation failed." << LogEnd;
      } else {
        Log(Debug) << "LQ: LQ-QUERY response generation successful." << LogEnd;
      }IsDone = true;
  } else {
      if (!answer(query)) {
        Log(Error) << "BLQ: LQ-QUERY response generation failed." << LogEnd;
      } else {
        Log(Debug) << "BLQ: LQ-QUERY response generation successful." << LogEnd;
      }
      IsDone = true;
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
    SPtr<TOpt> subOpt;
    bool send = false;
    if(!queryMsg->Bulk)
        Log(Info) << "LQ: Generating new LEASEQUERY-REPLY message." << LogEnd;
    else
        Log(Info) <<"BLQ: Generating new Bulk LEASEQUERY-REPLY message" << LogEnd;

    // copy CLIENT-ID
    opt = queryMsg->getOption(OPTION_CLIENTID);
    if (!opt) {
        Log(Error) << "LQ: query does not have client-id. Malformed." << LogEnd;
        IsDone = true;
        return true;
    }

    if (!count) {
        Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required LQ_QUERY option missing.", this));
	return true;
    }

    Options.push_back(opt);

    // append SERVERID
    SPtr<TOpt> serverID(new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this));
    Options.push_back(serverID);

    opt = queryMsg->getOption(OPTION_LQ_QUERY);
    if (opt) {


    queryMsg->firstOption();
    while (opt = queryMsg->getOption()) {
        if(opt->getOptType()== OPTION_LQ_QUERY) {
            subOpt = (Ptr*) queryMsg;
            break;
        }
    }
    subOpt = queryMsg->getOption(OPTION_LQ_QUERY);

    SPtr<TSrvOptLQ> q = (Ptr*) opt;
    q->getOptType();
    subOpt->firstOption();

    while (count < subOpt->countOption()+1) {

        count++;
        subOpt = subOpt->getOption();
        Log(Info) << "Following subOption has benn found:"<< subOpt->getOptType() <<LogEnd;
        if (!queryMsg->Bulk &&
            (subOpt->getOptType() == QUERY_BY_RELAY_ID ||
             subOpt->getOptType() == QUERY_BY_LINK_ADDRESS ||
             subOpt->getOptType() == QUERY_BY_REMOTE_ID) ) {
            Options.push_back( new TOptStatusCode(STATUSCODE_NOTALLOWED,
                                                  "You tried Bulk Leasequery over UDP. Please use TCP.",
                                                  this) );
            Log(Warning) << "LQ: Tried bulk leasequery query type over UDP. Please use TCP instead." << LogEnd;
            return true;
        }
        SPtr<TSrvOptLQ> q = (Ptr*) subOpt;
        switch (subOpt->getOptType()) {
        case QUERY_BY_ADDRESS:
            send = queryByAddress(q, queryMsg);
            break;
        case QUERY_BY_CLIENT_ID:
            send = queryByClientID(q, queryMsg);
            break;
        case QUERY_BY_LINK_ADDRESS:
            send = queryByLinkAddress(q, queryMsg);
            break;
        case QUERY_BY_RELAY_ID:
            send = queryByRelayID(q, queryMsg);
            break;
        case QUERY_BY_REMOTE_ID:
            send = queryByRemoteID(q, queryMsg);
            break;
        default:
            Options.push_back( new TOptStatusCode(STATUSCODE_UNKNOWNQUERYTYPE, "Invalid Query type.", this) );
            Log(Warning) << "LQ: Invalid query type (" << q->getQueryType() << " received." << LogEnd;
            return true;
        }
    }

    if (send) {
        // allocate buffer
        pkt = new char[getSize()];
        this->send();
    }

    return true;
}

bool TSrvMsgLeaseQueryReply::queryByAddress(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    SPtr<TOpt> opt;
    q->firstOption();
    SPtr<TIPv6Addr> link = q->getLinkAddr();
    SPtr<TSrvOptIAAddress> addr = 0;

    while ( opt = q->getOption() ) {
	if (opt->getOptType() == OPTION_IAADDR) {
	    addr = SPtr_cast<TSrvOptIAAddress>(opt);
            break;
        }
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
	    duidOpt = SPtr_cast<TOptDUID>(opt);
            if (duidOpt) {
                duid = duidOpt->getDUID();
                break;
            }
	}
    }
    if (!duid) {
	Options.push_back(new TOptStatusCode(STATUSCODE_UNSPECFAIL,
                                             "You didn't send your ClientID.", this) );
	return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( duid );

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client duid=" << duid->getPlain() << " not found." << LogEnd;
        Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this DUID found.", this) );
        return true;
    }

    appendClientData(cli);
    return true;
}

bool TSrvMsgLeaseQueryReply::queryByLinkAddress(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    SPtr<TOpt> opt;
    q->firstOption();
    SPtr<TSrvOptIAAddress> addr = 0;
    SPtr<TIPv6Addr> link = q->getLinkAddr();

    while ( opt = q->getOption() ) {
        if (opt->getOptType() == OPTION_IAADDR)
            addr = (Ptr*) opt;
    }
    if (!addr) {
        Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required IAADDR suboption missing.", this));
        return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( addr->getAddr() );

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client addr=" << addr->getAddr()->getPlain() << " not found." << LogEnd;
        Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this address found.", this) );
        return true;
    }

    appendClientData(cli);
    return true;
}

bool TSrvMsgLeaseQueryReply::queryByRemoteID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {

    SPtr<TOpt> opt;
    q->firstOption();

    SPtr<TOptRemoteID> remotePtr = 0;
    SPtr<TOptRemoteID> remoteId = 0;
    //SPtr<TIPv6Addr> link = q->getLinkAddr();

    while ( opt = q->getOption() ) {
        if (opt->getOptType() == OPTION_REMOTE_ID)
            remotePtr = (Ptr*) opt;
            remoteId = remotePtr->getRemoteId();
    }
    if (!remoteId) {
        Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required RemoteId suboption missing.", this));
        return true;
    }


    // search for client using existing RemoteId
    //TODO: how to return bindings via RemoteId by AddrMgr ?
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient();
    if (!cli) {
        Log(Warning) << "LQ: Assignement for client RemoteId=" << remoteId->getPlain() << " not found." << LogEnd;
        Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this address found.", this) );
        return true;
    }

    appendClientData(cli);
    return true;

    // algorithm:
    // search thru AddrMgr
    // for each matching client:
    //
    // if (first-match) {
    //      appendClientData(cli);
    //      sendTCP(); // send this message
    // } else {
    //      msg = new TSrvOptLeaseQueryData(queryMsg);
    //      msg->appendClientData(cli);
    //      msg->sendTCP();
    // }
    // msg = new TSrvOptLaseQueryDone(queryMsg);
    // msg->sendTCP();
    return false;
}

bool TSrvMsgLeaseQueryReply::queryByRelayID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    /// @todo: Implement query by relay-id


    SPtr<TOpt> opt;
    SPtr<TOptDUID> relayDuidOpt = 0;
    SPtr<TDUID> duid = 0;
   // SPtr<TIPv6Addr> link = q->getLinkAddr();

    q->firstOption();
    while ( opt = q->getOption() ) {
        if (opt->getOptType() == OPTION_CLIENTID) {
            relayDuidOpt = (Ptr*) opt;
            duid = relayDuidOpt->getDUID();
        }
    }
    if (!duid) {
    Options.push_back( new TOptStatusCode(STATUSCODE_UNSPECFAIL, "You didn't send your relay DUID.", this) );
    return true;
    }

    // search for client by duid
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( duid );

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client duid=" << duid->getPlain() << " not found." << LogEnd;
        Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this Relay DUID found.", this) );
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
            cliData->addOption( new TSrvOptIAPrefix( prefix->getPrefix(), prefix->getLength(), prefix->getPref(),
                                                     prefix->getValid(), this));
        }
    }

    cliData->addOption(new TOptDUID(OPTION_CLIENTID, cli->getDUID(), this));

    /// @todo: add all temporary addresses

    // add CLT_TIME
    Log(Debug) << "LQ: Adding CLT_TIME option: " << diff << " second(s)." << LogEnd;

    cliData->addOption( new TSrvOptLQClientTime(diff, this));

    Options.push_back(SPtr_cast<TOpt>(cliData));
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

bool TSrvMsgLeaseQueryReply::validateMsg(SPtr<TSrvMsgLeaseQuery> queryMsg)
{
    int failCount=0;
    SPtr<TOpt> opt;
    opt = queryMsg->getOption(OPTION_LQ_QUERY);

    if (!opt) {
        Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required LQ_QUERY option missing.", this));
        failCount++;
    }

    // copy CLIENT-ID
    opt = queryMsg->getOption(OPTION_CLIENTID);
    if (!opt) {
        Log(Error) << "LQ: query does not have client-id. Malformed." << LogEnd;
        if(!failCount) {
            Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required LQ_QUERY option missing.", this));
        }
        IsDone = true;
        failCount++;
    }
    Options.push_back(opt);

    // append SERVERID
    SPtr<TOptDUID> serverID;
    serverID = new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this);
    Options.push_back((Ptr*)serverID);

    if(failCount)
       return false;
    else
        return true;
}

string TSrvMsgLeaseQueryReply::getName() const {
    return "LEASEQUERY-REPLY";
}
