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

#include <cmath>
#include "SmartPtr.h"
#include "OptEmpty.h" // rapid-commit option
#include "SrvMsgReply.h"
#include "SrvMsg.h"
#include "OptOptionRequest.h"
#include "OptStatusCode.h"
#include "SrvOptIAAddress.h"
#include "SrvOptIA_NA.h"
#include "SrvOptIA_PD.h"
#include "SrvOptTA.h"
#include "SrvOptFQDN.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "IfaceMgr.h"
#include "Logger.h"

using namespace std;

/**
 * this constructor is used to create REPLY message as a response for CONFIRM message
 *
 * @param confirm
 */
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgConfirm> confirm)
    :TSrvMsg(confirm->getIface(),confirm->getRemoteAddr(), REPLY_MSG,
             confirm->getTransID())
{
    getORO( (Ptr*)confirm );
    copyClientID((Ptr*)confirm );
    copyRelayInfo((Ptr*)confirm);
    copyAAASPI((Ptr*)confirm);
    copyRemoteID((Ptr*)confirm);

    if (!handleConfirmOptions( confirm->getOptLst() )) {
        IsDone = true;
        return;
    }

    appendMandatoryOptions(ORO);
    appendAuthenticationOption(ClientDUID);

    this->MRT_ = 31;
    IsDone = false;
}


bool TSrvMsgReply::handleConfirmOptions(TOptList & options) {

    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(Iface);
    if (!cfgIface) {
        Log(Crit) << "Msg received through not configured interface. "
            "Somebody call an exorcist!" << LogEnd;
        IsDone = true;
        return false;
    }

    EAddrStatus onLink = ADDRSTATUS_YES;
    int checkCnt = 0;

    TOptList::iterator opt = options.begin();
    while ( (opt!=options.end()) && (onLink==ADDRSTATUS_YES) ) {

        switch ( (*opt)->getOptType()) {
        case OPTION_IA_NA: {

            SPtr<TSrvOptIA_NA> ia = (Ptr*) (*opt);

            // now we check whether this IA exists in Server Address database or not.
            SPtr<TOpt> opt;
            ia->firstOption();
            while ((opt = ia->getOption()) && (onLink == ADDRSTATUS_YES) ) {
                if (opt->getOptType() != OPTION_IAADDR){
                    continue;
                }

                SPtr<TSrvOptIAAddress> optAddr = (Ptr*) opt;
                onLink = cfgIface->confirmAddress(IATYPE_IA, optAddr->getAddr());
                checkCnt++;
            }
            break;
        }
        case OPTION_IA_TA: {
            SPtr<TSrvOptTA> ta = (Ptr*) (*opt);

            SPtr<TOpt> opt;
            ta->firstOption();
            while ((opt = ta->getOption()) && (onLink == ADDRSTATUS_YES)) {
                if (opt->getOptType() != OPTION_IAADDR)
                    continue;

                SPtr<TSrvOptIAAddress> optAddr = (Ptr*) opt;
                onLink = cfgIface->confirmAddress(IATYPE_TA, optAddr->getAddr());
                checkCnt++;
            }
            break;
        }
        case OPTION_IA_PD: {
            SPtr<TSrvOptIA_PD> ta = (Ptr*) (*opt);

            SPtr<TOpt> opt;
            ta->firstOption();
            while ((opt = ta->getOption()) && (onLink == ADDRSTATUS_YES)) {
                if (opt->getOptType() != OPTION_IAPREFIX)
                    continue;

                SPtr<TSrvOptIAPrefix> optPrefix = (Ptr*) opt;
                onLink = cfgIface->confirmAddress(IATYPE_PD, optPrefix->getPrefix());
                checkCnt++;
            }
            break;
        }
        default:
            handleDefaultOption(*opt);
            break;
        }
        ++opt;
    }

    if (!checkCnt) {
        Log(Info) << "No addresses or prefixes in CONFIRM. Not sending reply." << LogEnd;
        return false;
    }

    switch (onLink) {
    case ADDRSTATUS_YES: {
        SPtr <TOptStatusCode> ptrCode =
            new TOptStatusCode(STATUSCODE_SUCCESS,
                               "Your addresses are correct for this link! Yay!",
                               this);
        Options.push_back( (Ptr*) ptrCode);
        return true;
    }

    case ADDRSTATUS_NO: {
        SPtr <TOptStatusCode> ptrCode =
            new TOptStatusCode(STATUSCODE_NOTONLINK,
                               "Sorry, those addresses are not valid for this link.",
                               this);
        Options.push_back( (Ptr*) ptrCode );
        return true;
    }

    default:
    case ADDRSTATUS_UNKNOWN: {
        Log(Info) << "Address/prefix being confirmed is outside of defined class,"
                  << " but there is no subnet defined, so can't answer authoritatively."
                  << " Will not send answer." << LogEnd;
        return false;
    }
    }

    // should never get here
    return false;
}


/*
 * this constructor is used to create REPLY message as a response for DECLINE message
 *
 * @param decline
 */
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgDecline> decline)
    :TSrvMsg(decline->getIface(), decline->getRemoteAddr(), REPLY_MSG,
             decline->getTransID())
{
    getORO( (Ptr*)decline );
    copyClientID( (Ptr*)decline );
    copyRelayInfo( (Ptr*)decline );
    copyAAASPI( (Ptr*)decline );
    copyRemoteID( (Ptr*)decline );

    SPtr<TOpt> ptrOpt;

    SPtr<TAddrClient> ptrClient = SrvAddrMgr().getClient(ClientDUID);
    if (!ptrClient) {
        Log(Warning) << "Received DECLINE from unknown client, DUID="
                     << *ClientDUID << ". Ignored." << LogEnd;
        IsDone = true;
        return;
    }

    SPtr<TDUID> declinedDUID = new TDUID("X",1);
    SPtr<TAddrClient> declinedClient = SrvAddrMgr().getClient( declinedDUID );
    if (!declinedClient) {
        declinedClient = new TAddrClient( declinedDUID );
        SrvAddrMgr().addClient(declinedClient);
    }

    decline->firstOption();
    while (ptrOpt = decline->getOption() )
    {
        switch (ptrOpt->getOptType())
        {
        case OPTION_IA_NA:
        {
            SPtr<TSrvOptIA_NA> ptrIA_NA = (Ptr*) ptrOpt;
            SPtr<TAddrIA> ptrIA = ptrClient->getIA(ptrIA_NA->getIAID());
            if (!ptrIA)
            {
                Options.push_back( new TSrvOptIA_NA(ptrIA_NA->getIAID(), 0, 0, STATUSCODE_NOBINDING,
                                             "No such IA is bound.",this) );
                continue;
            }

            // create empty IA
            SPtr<TSrvOptIA_NA> replyIA_NA = new TSrvOptIA_NA(ptrIA_NA->getIAID(), 0, 0, this);
            int AddrsDeclinedCnt = 0;

            // IA found in DB, now move each addr in DB to "declined-client" and
            // ignore those, which are not present id DB
            SPtr<TOpt> subOpt;
            SPtr<TSrvOptIAAddress> addr;
            ptrOpt->firstOption();
            while( subOpt = ptrOpt->getOption() ) {
                if (subOpt->getOptType()==OPTION_IAADDR)
                {
                    addr = (Ptr*) subOpt;

                    // remove declined address from client
                    if (SrvAddrMgr().delClntAddr(ptrClient->getDUID(), ptrIA_NA->getIAID(),
                                                 addr->getAddr(), false)) {

                        // add this address to DECLINED dummy client
                        SrvAddrMgr().addClntAddr(declinedDUID,
                                                 new TIPv6Addr("::", true), // client-address
                                                 decline->getIface(),
                                                 0, 0, 0, // IAID
                                                 addr->getAddr(), // declined address
                                                 DECLINED_TIMEOUT, DECLINED_TIMEOUT,
                                                 false);

                        // set pref/valid lifetimes to 0
                        addr->setValid(0);
                        addr->setPref(0);
                        // ... and finally append it in reply
                        replyIA_NA->addOption( subOpt );

                        AddrsDeclinedCnt++;
                    }
                };
            }
            Options.push_back((Ptr*)replyIA_NA);
            char buf[10];
            sprintf(buf,"%d",AddrsDeclinedCnt);
            string tmp = buf;
            SPtr<TOptStatusCode> optStatusCode =
                new TOptStatusCode(STATUSCODE_SUCCESS, tmp + " addrs declined.", this);
            replyIA_NA->addOption( (Ptr*) optStatusCode );
            break;
        };
        case OPTION_IA_TA:
            Log(Info) << "TA address declined. It's temporary, so let's ignore it entirely."
                      << LogEnd;
            break;
        default:
            handleDefaultOption(ptrOpt);
            break;
        }
    }

    appendMandatoryOptions(ORO);

    appendAuthenticationOption(ClientDUID);

    IsDone = false;
    MRT_ = 31;
}

/**
 * this constructor is used to create REPLY message as a response for REBIND message
 *
 * @param rebind
 */
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgRebind> rebind)
    :TSrvMsg(rebind->getIface(),rebind->getRemoteAddr(), REPLY_MSG, rebind->getTransID())
{
    getORO( (Ptr*)rebind );
    copyClientID( (Ptr*)rebind );
    copyRelayInfo( (Ptr*)rebind );
    copyAAASPI( (Ptr*)rebind );
    copyRemoteID( (Ptr*)rebind );

    unsigned long addrCount=0;
    SPtr<TOpt> ptrOpt;

    rebind->firstOption();
    while (ptrOpt = rebind->getOption() )
    {
        switch (ptrOpt->getOptType())
        {
        case OPTION_IA_NA:
          {
            SPtr<TSrvOptIA_NA> optIA_NA;
            optIA_NA = new TSrvOptIA_NA((Ptr*)ptrOpt,
                                        rebind->getRemoteAddr(), ClientDUID,
                                        rebind->getIface(), addrCount, REBIND_MSG,
                                        this);
            if (optIA_NA->getStatusCode() != STATUSCODE_NOBINDING )
              Options.push_back((Ptr*)optIA_NA);
            else {
                this->IsDone = true;
                Log(Notice) << "REBIND received with unknown addresses and "
                            << "was silently discarded." << LogEnd;
              return;
            }
            break;
          }
        case OPTION_IA_PD: {
            SPtr<TSrvOptIA_PD> pd;
            pd = new TSrvOptIA_PD( (Ptr*)rebind, (Ptr*) ptrOpt, this);
            Options.push_back((Ptr*)pd);
            break;
        }

        case OPTION_IAADDR:
        case OPTION_RAPID_COMMIT:
        case OPTION_STATUS_CODE:
        case OPTION_PREFERENCE:
        case OPTION_UNICAST:
            Log(Warning) << "Invalid option (" <<ptrOpt->getOptType() << ") received." << LogEnd;
            break;
        default:
            handleDefaultOption(ptrOpt);
            break;
        }
    }

    appendMandatoryOptions(ORO);
    appendRequestedOptions(ClientDUID, rebind->getRemoteAddr(),rebind->getIface(), ORO);
    appendAuthenticationOption(ClientDUID);

    IsDone = false;
    MRT_ = 0;
}

/**
 * this constructor is used to create REPLY message as a response for RELEASE message
 *
 * @param release
 */
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgRelease> release)
    :TSrvMsg(release->getIface(),release->getRemoteAddr(), REPLY_MSG,
             release->getTransID())
{
    getORO( (Ptr*) release );
    copyClientID( (Ptr*) release );
    copyRelayInfo((Ptr*)release);
    copyAAASPI((Ptr*)release);
    copyRemoteID((Ptr*)release);

    /// @todo When the server receives a Release message via unicast from a client
    /// to which the server has not sent a unicast option, the server
    /// discards the Release message and responds with a Reply message
    /// containing a Status Code option with value UseMulticast, a Server
    /// Identifier option containing the server's DUID, the Client Identifier
    /// option from the client message, and no other options.

    // for notify script
    TNotifyScriptParams* notifyParams = new TNotifyScriptParams();

    SPtr<TOpt> opt, subOpt;

    SPtr<TAddrClient> client = SrvAddrMgr().getClient(ClientDUID);
    if (!client) {
        Log(Warning) << "Received RELEASE from unknown client DUID=" << ClientDUID->getPlain() << LogEnd;
        IsDone = true;
        return;
    }

    SPtr<TSrvCfgIface> ptrIface = SrvCfgMgr().getIfaceByID( this->Iface );
    if (!ptrIface) {
        Log(Crit) << "Msg received through not configured interface. "
            "Somebody call an exorcist!" << LogEnd;
        IsDone = true;
        return;
    }

    appendMandatoryOptions(ORO);
    appendAuthenticationOption(ClientDUID);

    release->firstOption();
    while(opt=release->getOption()) {
        switch (opt->getOptType()) {
        case OPTION_IA_NA: {
            SPtr<TSrvOptIA_NA> clntIA = (Ptr*) opt;
            SPtr<TSrvOptIAAddress> addr;
            bool anyDeleted=false;

            // does this client has IA? (iaid check)
            SPtr<TAddrIA> ptrIA = client->getIA(clntIA->getIAID() );
            if (!ptrIA) {
                Log(Warning) << "No such IA (iaid=" << clntIA->getIAID() << ") found for client:" << ClientDUID->getPlain() << LogEnd;
                Options.push_back( new TSrvOptIA_NA(clntIA->getIAID(), 0, 0, STATUSCODE_NOBINDING,"No such IA is bound.",this) );
                continue;
            }

            // if there was DNS Update performed, execute deleting Update
            SPtr<TFQDN> fqdn = ptrIA->getFQDN();
            if (fqdn) {
                delFQDN(ptrIface, ptrIA, fqdn);
            }

            // let's verify each address
            clntIA->firstOption();
            while(subOpt=clntIA->getOption()) {
                if (subOpt->getOptType()!=OPTION_IAADDR)
                    continue;
                addr = (Ptr*) subOpt;
                if (SrvAddrMgr().delClntAddr(ClientDUID, clntIA->getIAID(), addr->getAddr(), false) ) {
                    notifyParams->addAddr(addr->getAddr(), 0, 0, "SRV");

                    SrvCfgMgr().delClntAddr(this->Iface,addr->getAddr());
                    anyDeleted=true;
                } else {
                    Log(Warning) << "No such binding found: client=" << ClientDUID->getPlain() << ", IA (iaid="
                                 << clntIA->getIAID() << "), addr="<< addr->getAddr()->getPlain() << LogEnd;
                };
            };

            // send result to the client
            if (!anyDeleted)
            {
                SPtr<TSrvOptIA_NA> ansIA(new TSrvOptIA_NA(clntIA->getIAID(), clntIA->getT1(),clntIA->getT2(),this));
                Options.push_back((Ptr*)ansIA);
                ansIA->addOption(new TOptStatusCode(STATUSCODE_NOBINDING, "Not every address had binding.",this));
            };
            break;
        }
        case OPTION_IA_TA: {
            SPtr<TSrvOptTA> ta = (Ptr*) opt;
            bool anyDeleted = false;
            SPtr<TAddrIA> ptrIA = client->getTA(ta->getIAID() );
            if (!ptrIA) {
                Log(Warning) << "No such TA (iaid=" << ta->getIAID() << ") found for client:" << ClientDUID->getPlain() << LogEnd;
                Options.push_back( new TSrvOptTA(ta->getIAID(), STATUSCODE_NOBINDING, "No such IA is bound.", this) );
                continue;
            }

            // let's verify each address
            ta->firstOption();
            while ( subOpt = (Ptr*) ta->getOption() ) {
                if (subOpt->getOptType()!=OPTION_IAADDR)
                    continue;
                SPtr<TSrvOptIAAddress> addr = (Ptr*) subOpt;
                if (SrvAddrMgr().delTAAddr(ClientDUID, ta->getIAID(), addr->getAddr(), false) ) {
                    notifyParams->addAddr(addr->getAddr(), 0, 0 , "");

                    SrvCfgMgr().delTAAddr(this->Iface);
                    anyDeleted=true;
                } else {
                    Log(Warning) << "No such binding found: client=" << ClientDUID->getPlain() << ", TA (iaid="
                             << ta->getIAID() << "), addr="<< addr->getAddr()->getPlain() << LogEnd;
                };
            }

            // send results to the client
            if (!anyDeleted)
            {
                SPtr<TSrvOptTA> answerTA = new TSrvOptTA(ta->getIAID(), STATUSCODE_NOBINDING, "Not every address had binding.", this);
                Options.push_back((Ptr*)answerTA);
            };
            break;
        }

        case OPTION_IA_PD: {
            SPtr<TSrvOptIA_PD> pd = (Ptr*) opt;
            SPtr<TSrvOptIAPrefix> prefix;
            bool anyDeleted=false;

            // does this client has PD? (iaid check)
            SPtr<TAddrIA> ptrPD = client->getPD( pd->getIAID() );
            if (!ptrPD) {
                Log(Warning) << "No such PD (iaid=" << pd->getIAID() << ") found for client:" << ClientDUID->getPlain() << LogEnd;
                Options.push_back( new TSrvOptIA_PD(pd->getIAID(), 0, 0, STATUSCODE_NOBINDING,"No such PD is bound.",this) );
                continue;
            }

            // let's verify each address
            pd->firstOption();
            while(subOpt=pd->getOption()) {
                if (subOpt->getOptType()!=OPTION_IAPREFIX)
                    continue;
                prefix = (Ptr*) subOpt;
                if (SrvAddrMgr().delPrefix(ClientDUID, pd->getIAID(), prefix->getPrefix(), false) ) {
                    notifyParams->addPrefix(prefix->getPrefix(), prefix->getPrefixLength(), 0, 0);
                    SrvCfgMgr().decrPrefixCount(Iface, prefix->getPrefix());
                    anyDeleted=true;
                } else {
                    Log(Warning) << "PD: No such binding found: client=" << ClientDUID->getPlain() << ", PD (iaid="
                                 << pd->getIAID() << "), addr="<< prefix->getPrefix()->getPlain() << LogEnd;
                };
            };

            // send result to the client
            if (!anyDeleted)
            {
                Options.push_back(new TSrvOptIA_PD(pd->getIAID(), 0u, 0u, 
                                                   STATUSCODE_NOBINDING, "Not every address had binding.", this));
            };
            break;
        }
        default:
            handleDefaultOption(opt);
            break;
        }; // switch(...)
    } // while

    Options.push_back(new TOptStatusCode(STATUSCODE_SUCCESS,
                                         "All IAs in RELEASE message were processed.",this));

    NotifyScripts = notifyParams;

    IsDone = false;
    MRT_ = 46;
}

// used as RENEW reply
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgRenew> renew)
    :TSrvMsg(renew->getIface(),renew->getRemoteAddr(), REPLY_MSG, renew->getTransID())
{
    getORO( (Ptr*)renew );
    copyClientID( (Ptr*)renew );
    copyRelayInfo((Ptr*)renew);
    copyAAASPI((Ptr*)renew);
    copyRemoteID((Ptr*)renew);

    unsigned long addrCount=0;
    SPtr<TOpt> ptrOpt;

    renew->firstOption();
    while (ptrOpt = renew->getOption() )
    {
        switch (ptrOpt->getOptType())
        {
        case OPTION_IA_NA: {
            SPtr<TSrvOptIA_NA> optIA_NA;
            optIA_NA = new TSrvOptIA_NA((Ptr*)ptrOpt,
                                        renew->getRemoteAddr(), ClientDUID,
                                        renew->getIface(), addrCount, RENEW_MSG, this);
            Options.push_back((Ptr*)optIA_NA);
            break;
        }
        case OPTION_IA_PD: {
            SPtr<TSrvOptIA_PD> optPD;
            optPD = new TSrvOptIA_PD((Ptr*) renew, (Ptr*)ptrOpt, this);
            Options.push_back( (Ptr*) optPD);
            break;
        }
        case OPTION_IA_TA:
            Log(Warning) << "TA option present. Temporary addreses cannot be renewed." << LogEnd;
            break;
        //Invalid options in RENEW message
        case OPTION_RELAY_MSG :
        case OPTION_INTERFACE_ID :
        case OPTION_RECONF_MSG:
        case OPTION_IAADDR:
        case OPTION_PREFERENCE:
        case OPTION_RAPID_COMMIT:
        case OPTION_UNICAST:
        case OPTION_STATUS_CODE:
            Log(Warning) << "Invalid option "<<ptrOpt->getOptType()<<" received." << LogEnd;
            break;
        default:
            handleDefaultOption(ptrOpt);
            // do nothing with remaining options
            break;
        }
    }

    appendMandatoryOptions(ORO);
    appendRequestedOptions(ClientDUID,renew->getRemoteAddr(),renew->getIface(), ORO);
    appendAuthenticationOption(ClientDUID);

    IsDone = false;
    MRT_ = 0;
}

/**
 * this constructor is used to construct REPLY for REQUEST message
 *
 * @param request
 */
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgRequest> request)
    :TSrvMsg(request->getIface(), request->getRemoteAddr(), REPLY_MSG, request->getTransID())
{
    getORO( (Ptr*)request );
    copyClientID( (Ptr*)request );
    copyRelayInfo( (Ptr*)request );
    copyAAASPI( (Ptr*)request );
    copyRemoteID( (Ptr*)request );

    processOptions((Ptr*)request, false); // be verbose

    appendMandatoryOptions(ORO);
    appendRequestedOptions(ClientDUID, PeerAddr_, Iface, ORO);
    appendAuthenticationOption(ClientDUID);

#ifndef MOD_DISABLE_AUTH
    SPtr<TOpt> reconfAccept = request->getOption(OPTION_RECONF_ACCEPT);
    if (reconfAccept) {
        appendReconfigureKey();
    }
#endif

    IsDone = false;
    MRT_ = 330;
}

/// @brief ctor used for generating REPLY message as SOLICIT response (in rapid-commit mode)
///
/// @param solicit client's message
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgSolicit> solicit)
    :TSrvMsg(solicit->getIface(), solicit->getRemoteAddr(), REPLY_MSG, solicit->getTransID())
{
    getORO( (Ptr*)solicit );
    copyClientID( (Ptr*)solicit );
    copyRelayInfo( (Ptr*)solicit );
    copyAAASPI( (Ptr*)solicit );
    copyRemoteID( (Ptr*)solicit );

    processOptions((Ptr*) solicit, false);

    // append RAPID-COMMIT option
    Options.push_back(new TOptEmpty(OPTION_RAPID_COMMIT, this));

    appendMandatoryOptions(ORO);
    appendRequestedOptions(ClientDUID, PeerAddr_, Iface, ORO);
    appendAuthenticationOption(ClientDUID);

    IsDone = false;
    MRT_ = 330;
}

// INFORMATION-REQUEST answer
TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsgInfRequest> infRequest)
    :TSrvMsg(infRequest->getIface(),
             infRequest->getRemoteAddr(),REPLY_MSG,infRequest->getTransID())
{
    getORO( (Ptr*)infRequest );
    copyClientID( (Ptr*)infRequest );
    copyRelayInfo((Ptr*)infRequest);
    copyAAASPI((Ptr*)infRequest);
    copyRemoteID((Ptr*)infRequest);

    Log(Debug) << "Received INF-REQUEST requesting " << showRequestedOptions(ORO) << "." << LogEnd;

    infRequest->firstOption();
    SPtr<TOpt> ptrOpt;
    while (ptrOpt = infRequest->getOption() )
    {
        switch (ptrOpt->getOptType())
        {

            case OPTION_RELAY_MSG   :
            case OPTION_SERVERID    :
            case OPTION_INTERFACE_ID:
            case OPTION_STATUS_CODE :
            case OPTION_IAADDR      :
            case OPTION_PREFERENCE  :
            case OPTION_UNICAST     :
            case OPTION_RECONF_MSG  :
            case OPTION_IA_NA       :
            case OPTION_IA_TA       :
                Log(Warning) << "Invalid option " << ptrOpt->getOptType() <<" received." << LogEnd;
                break;
            default:
                handleDefaultOption(ptrOpt);
            break;
        }
    }

    appendMandatoryOptions(ORO);
    if ( !appendRequestedOptions(ClientDUID, infRequest->getRemoteAddr(),
                                 infRequest->getIface(), ORO) ) {
        Log(Warning) << "No options to answer in INF-REQUEST, so REPLY will not be send." << LogEnd;
        IsDone=true;
        return;
    }

    appendAuthenticationOption(ClientDUID);

    IsDone = false;
    MRT_ = 330;
}

TSrvMsgReply::TSrvMsgReply(SPtr<TSrvMsg> msg, TOptList& options)
    :TSrvMsg(msg->getIface(), msg->getRemoteAddr(), REPLY_MSG,
             msg->getTransID()) {

    // Let's just use specified options as they are
    Options = options;

    // Append client-id
    SPtr<TOpt> client_id = msg->getOption(OPTION_CLIENTID);
    if (client_id) {
        Options.push_back(client_id);
    }

    // Append server-id
    SPtr<TOptDUID> ptrSrvID;
    ptrSrvID = new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this);
    Options.push_back((Ptr*)ptrSrvID);

    MRT_ = 330;
    IsDone = false;
}

void TSrvMsgReply::doDuties() {
    IsDone = true;
}

unsigned long TSrvMsgReply::getTimeout() {
    unsigned long diff = (uint32_t)time(NULL) - FirstTimeStamp_;
    if (diff > SERVER_REPLY_CACHE_TIMEOUT)
        return 0;
    return SERVER_REPLY_CACHE_TIMEOUT - diff;
}

bool TSrvMsgReply::check() {
    /* not used on the server side */
    /* we generate REPLY messages, so they are *GOOD*. */
    return false;
}

TSrvMsgReply::~TSrvMsgReply()
{

}

string TSrvMsgReply::getName() const {
    return "REPLY";
}
