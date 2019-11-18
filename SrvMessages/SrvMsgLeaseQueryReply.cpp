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
#include <sstream>

using namespace std;

// was query->getAddr()
TSrvMsgLeaseQueryReply::TSrvMsgLeaseQueryReply(SPtr<TSrvMsgLeaseQuery> query)
    :TSrvMsg(query->getIface(), query->getRemoteAddr(), LEASEQUERY_REPLY_MSG,
             query->getTransID())
{
    string lq = (query->Bulk)?"BLQ":"LQ";

    if (!answer(query)) {
        Log(Error) << lq << ": LQ-QUERY response generation failed." << LogEnd;
    } else {
        Log(Debug) << lq << ": LQ-QUERY response generation successful." << LogEnd;
    }
    IsDone = true;
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
    bool result = false;
    Log(Info) << "LQ: Generating new " << (queryMsg->Bulk?"Bulk ":"") << "LEASEQUERY-REPLY message." << LogEnd;

    // Copy the CLIENT-ID.
    opt = queryMsg->getOption(OPTION_CLIENTID);
    if (!opt) {
        Log(Warning) << "LQ: query does not have client-id. Malformed." << LogEnd;
        IsDone = true;
        return true;
    }
    Options.push_back(opt);

    // Append the SERVER-ID.
    SPtr<TOpt> serverID(new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this));
    Options.push_back(serverID);

    // Find the lq-query option
    SPtr<TSrvOptLQ> lq;
    queryMsg->firstOption();
    while ((opt = queryMsg->getOption())) {
        if(opt->getOptType()== OPTION_LQ_QUERY) {
            count++;
            lq = SPtr_cast<TSrvOptLQ>(opt);
            break;
        }
    }
    if (count != 1) {
        stringstream tmp;
        tmp << "Required exactly 1 LQ_QUERY option, got " << count;
        Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, tmp.str(), this));
        return true;
    }

    if (!lq) {
        Log(Error) << "BLQ: Unable to find LQ_QUERY option." << LogEnd;
        return true;
    }

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

    switch (lq->getQueryType()) {
    case QUERY_BY_ADDRESS:
        result = queryByAddress(lq);
        break;
    case QUERY_BY_CLIENT_ID:
        result = queryByClientID(lq);
        break;
    case QUERY_BY_LINK_ADDRESS:
        result = queryByLinkAddress(lq);
        break;
    case QUERY_BY_RELAY_ID:
        result = queryByRelayID(lq);
        break;
    case QUERY_BY_REMOTE_ID:
        result = queryByRemoteID(lq);
        break;
    default:
        stringstream tmp;
        tmp << "Invalid query type (" << lq->getQueryType() << " received.";
        Options.push_back( new TOptStatusCode(STATUSCODE_UNKNOWNQUERYTYPE, tmp.str(), this) );
        Log(Warning) << "LQ: " << tmp.str() << LogEnd;
        return true;
    }

    if (result) {
        send();
    }

    return true;
}

bool TSrvMsgLeaseQueryReply::queryByAddress(SPtr<TSrvOptLQ> q) {
    SPtr<TOpt> opt;
    q->firstOption();
    SPtr<TSrvOptIAAddress> addr;

    while ((opt = q->getOption())) {
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

bool TSrvMsgLeaseQueryReply::queryByClientID(SPtr<TSrvOptLQ> q) {
    SPtr<TOpt> opt;
    SPtr<TOptDUID> duidOpt;
    SPtr<TDUID> duid;
    SPtr<TIPv6Addr> link = q->getLinkAddr();

    q->firstOption();
    while ((opt = q->getOption())) {
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

bool TSrvMsgLeaseQueryReply::queryByLinkAddress(SPtr<TSrvOptLQ> q) {
    SPtr<TOpt> opt;
    SPtr<TOptIAAddress> addr;

    // Find the address.
    q->firstOption();
    while ((opt = q->getOption())) {
        if (opt->getOptType() == OPTION_IAADDR)
            addr = SPtr_cast<TOptIAAddress>(opt);
    }
    if (!addr) {
        Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, "Required IAADDR suboption missing.", this));
        return true;
    }

    // Search for the client with specified address.
    SPtr<TAddrClient> cli = SrvAddrMgr().getClient( addr->getAddr() );

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client addr=" << addr->getAddr()->getPlain() << " not found." << LogEnd;
        Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this address found.", this) );
        return true;
    }

    appendClientData(cli);
    return true;
}

bool TSrvMsgLeaseQueryReply::queryByRemoteID(SPtr<TSrvOptLQ> q) {

    SPtr<TOpt> opt;

    SPtr<TOptRemoteID> remoteId;

    // Get the remote-id option
    int cnt = 0;
    q->firstOption();
    while ((opt = q->getOption())) {
        if (opt->getOptType() == OPTION_REMOTE_ID) {
            remoteId = SPtr_cast<TOptRemoteID>(opt);
            cnt++;
        }
    }
    if (cnt != 1) {
        stringstream tmp;
        tmp << "Expected to get exactly one remote-id option, got " << cnt;
        Options.push_back(new TOptStatusCode(STATUSCODE_MALFORMEDQUERY, tmp.str(), this));
        return true;
    }
    if (!remoteId) {
        Log(Error) << "Failed to cast remote-id." << LogEnd;
        return false;
    }

    // Now find a client with specified remote-id
    cnt = 0;
    SrvAddrMgr().firstClient();
    SPtr<TAddrClient> cli;
    while ((cli = SrvAddrMgr().getClient())) {
        /// @todo: there remote-id information is currently not retained in the AddrMgr. No way to check it.
        if (false)
            appendClientData(cli);
    }

    if (!cli) {
        Log(Warning) << "LQ: Assignement for client RemoteId=" << remoteId->getPlain() << " not found." << LogEnd;
        Options.push_back( new TOptStatusCode(STATUSCODE_NOTCONFIGURED, "No binding for this address found.", this) );
        return true;
    }

    return true;
}

bool TSrvMsgLeaseQueryReply::queryByRelayID(SPtr<TSrvOptLQ> q) {
    /// @todo: Implement query by relay-id
    string error = "Query by relay-id is not implemented yet.";
    Log(Error) << error << LogEnd;
    Options.push_back( new TOptStatusCode(STATUSCODE_UNSPECFAIL, error, this) );
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
    while (( ia = cli->getIA() )) {
        ia->firstAddr();
        while ((addr=ia->getAddr())) {
            unsigned long a = addr->getPref() - diff;
            unsigned long b = addr->getValid() - diff;
            cliData->addOption( new TOptIAAddress(addr->get(), a, b, this) );
        }
    }

    // add all assigned prefixes
    cli->firstPD();
    while ((ia = cli->getPD())) {
        ia->firstPrefix();
        while ((prefix = ia->getPrefix())) {
            cliData->addOption(new TOptIAPrefix(prefix->get(), prefix->getLength(), prefix->getPref(),
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
    Options.push_back(new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this));

    if(failCount)
       return false;
    else
        return true;
}

string TSrvMsgLeaseQueryReply::getName() const {
    return "LEASEQUERY-REPLY";
}
