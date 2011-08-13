/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgLeaseQueryReply.cpp,v 1.7 2008-08-29 00:07:35 thomson Exp $
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
#include "SrvCfgMgr.h"


TSrvMsgLeaseQueryReply::TSrvMsgLeaseQueryReply(SPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsg(query->getIface(), query->getAddr(), LEASEQUERY_REPLY_MSG,
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
    bool send = true;

    Log(Info) << "LQ: Generating new LEASEQUERY_REPLY message." << LogEnd;

    // copy CLIENT-ID
    opt = queryMsg->getOption(OPTION_CLIENTID);
    if (!opt) {
        Log(Error) << "BLQ: query does not have client-id. Malformed." << LogEnd;
        IsDone = true;
        return true;
    }
    Options.push_back(opt);

    // append SERVER-ID
    SPtr<TSrvOptServerIdentifier> ptrSrvID;
    ptrSrvID = new TSrvOptServerIdentifier(SrvCfgMgr().getDUID(), this);
    Options.push_back((Ptr*)ptrSrvID);

    opt = queryMsg->getOption(OPTION_LQ_QUERY);
    if (opt) {
        count++;
        SPtr<TSrvOptLQ> q = (Ptr*) opt;
        if (!queryMsg->isTCP() &&
            (q->getQueryType() == QUERY_BY_RELAY_ID ||
             q->getQueryType() == QUERY_BY_LINK_ADDRESS ||
             q->getQueryType() == QUERY_BY_REMOTE_ID) ) {
            Options.push_back( new TSrvOptStatusCode(STATUSCODE_NOTALLOWED,
                                                     "You tried Bulk Leasequery over UDP. Please use TCP.",
                                                     this) );
            Log(Warning) << "LQ: Tried bulk leasequery query type over UDP. Please use TCP instead." << LogEnd;
            return true;
        }

        switch (q->getQueryType()) {
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
            Options.push_back( new TSrvOptStatusCode(STATUSCODE_UNKNOWNQUERYTYPE, "Invalid Query type.", this) );
            Log(Warning) << "LQ: Invalid query type (" << q->getQueryType() << " received." << LogEnd;
            return true;
        }
    }

    if (!count) {
        Options.push_back(new TSrvOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required LQ_QUERY option missing.", this));
        return true;
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
    SPtr<TSrvOptIAAddress> addr = 0;
    SPtr<TIPv6Addr> link = q->getLinkAddr();

    while ( opt = q->getOption() ) {
        if (opt->getOptType() == OPTION_IAADDR)
            addr = (Ptr*) opt;
    }
    if (!addr) {
        Options.push_back(new TSrvOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required IAADDR suboption missing.", this));
        return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( addr->getAddr() );

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client addr=" << addr->getAddr()->getPlain() << " not found." << LogEnd;
        Options.push_back( new TSrvOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this address found.", this) );
        return true;
    }

    appendClientData(cli);
    return true;
}

bool TSrvMsgLeaseQueryReply::queryByClientID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    SPtr<TOpt> opt;
    SPtr<TSrvOptClientIdentifier> duidOpt = 0;
    SPtr<TDUID> duid = 0;
    SPtr<TIPv6Addr> link = q->getLinkAddr();

    q->firstOption();
    while ( opt = q->getOption() ) {
        if (opt->getOptType() == OPTION_CLIENTID) {
            duidOpt = (Ptr*) opt;
            duid = duidOpt->getDUID();
        }
    }
    if (!duid) {
        Options.push_back( new TSrvOptStatusCode(STATUSCODE_UNSPECFAIL, "You didn't send your ClientID.", this) );
        return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( duid );

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client duid=" << duid->getPlain() << " not found." << LogEnd;
        Options.push_back( new TSrvOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this DUID found.", this) );
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
        Options.push_back(new TSrvOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required IAADDR suboption missing.", this));
        return true;
    }

    // search for client
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( addr->getAddr() );

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client addr=" << addr->getAddr()->getPlain() << " not found." << LogEnd;
        Options.push_back( new TSrvOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this address found.", this) );
        return true;
    }

    appendClientData(cli);
    return true;
}

bool TSrvMsgLeaseQueryReply::queryByRemoteID(SPtr<TSrvOptLQ> q, SPtr<TSrvMsgLeaseQuery> queryMsg) {
    // TODO
    Log(Error) << "BLQ: Query by Remote-ID not implemented yet." << LogEnd;

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
    // TODO

    Log(Error) << "BLQ: Query by Relay-ID not implemented yet." << LogEnd;
    return false;
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

    cliData->addOption(new TSrvOptClientIdentifier(cli->getDUID(), this));

    // TODO: add all temporary addresses

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

string TSrvMsgLeaseQueryReply::getName() {
    return "LEASE-QUERY-REPLY";
}
