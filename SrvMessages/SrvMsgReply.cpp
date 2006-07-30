/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *                                                                           
 */

#include <cmath>
#include "SmartPtr.h"
#include "SrvMsgReply.h"
#include "SrvMsg.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptStatusCode.h"
#include "SrvOptIAAddress.h"
#include "SrvOptIA_NA.h"
#include "SrvOptTA.h"
#include "SrvOptRapidCommit.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptTimeZone.h"
#include "SrvOptDomainName.h"
#include "SrvOptDNSServers.h"
#include "SrvOptNTPServers.h"
#include "SrvOptFQDN.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "Logger.h"
#include "DNSUpdate.h"

// used as CONFIRM reply
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgConfirm> confirm)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     confirm->getIface(),confirm->getAddr(), REPLY_MSG, 
	     confirm->getTransID())
{
    this->copyRelayInfo((Ptr*)confirm);
    SmartPtr<TSrvCfgIface> ptrIface = CfgMgr->getIfaceByID( confirm->getIface() );
    if (!ptrIface) {
	Log(Crit) << "Msg received through not configured interface. "
	    "Somebody call an exorcist!" << LogEnd;
	this->IsDone = true;
	return;
    }

    // copy client's DUID
    SmartPtr<TOpt> ptrOpt;
    SmartPtr<TSrvOptClientIdentifier> ptrClntDUID;
    ptrOpt = confirm->getOption(OPTION_CLIENTID);
    ptrClntDUID = (Ptr*) ptrOpt;

    // include our DUID
    ptrOpt = confirm->getOption(OPTION_CLIENTID);
    Options.append(ptrOpt);

    confirm->firstOption();
    bool OnLink = true;
    while (ptrOpt=confirm->getOption() && (OnLink) ) {
	switch (ptrOpt->getOptType()) {
	case OPTION_IA: {
	    SmartPtr<TSrvOptIA_NA> ia = (Ptr*) ptrOpt;
	    SmartPtr<TOpt> subOpt;
	    ia->firstOption();
	    while ( subOpt = ia->getOption() && (OnLink) ) {
		if (subOpt->getOptType() != OPTION_IAADDR)
		    continue;
		SmartPtr<TSrvOptIAAddress> optAddr = (Ptr*) subOpt;
		if (!CfgMgr->isIAAddrSupported(this->Iface, ia->getIAID(), optAddr->getAddr())) {
		    OnLink = false;
		}
	    }
	    break;
	}
	case OPTION_IA_TA: {
	    SmartPtr<TSrvOptTA> ta = (Ptr*) ptrOpt;
	    SmartPtr<TOpt> subOpt;
	    ta->firstOption();
	    while (subOpt = ta->getOption() && (OnLink)) {
		if (subOpt->getOptType() != OPTION_IAADDR)
		    continue;
		SmartPtr<TSrvOptIAAddress> optAddr = (Ptr*) subOpt;
		if (!CfgMgr->isTAAddrSupported(this->Iface, ta->getIAID(), optAddr->getAddr())) {
		    OnLink = false;
		}

	    }
	    break;
	}
	default:
	    // other options - ignore them
	    continue;
	}
    }
    if (!OnLink) {
	SmartPtr <TSrvOptStatusCode> ptrCode = 
	    new TSrvOptStatusCode(STATUSCODE_NOTONLINK,
				  "Sorry, those addresses are not valid for this link.",
				  this);
	this->Options.append( (Ptr*) ptrCode );
    } else {
	SmartPtr <TSrvOptStatusCode> ptrCode = 
	    new TSrvOptStatusCode(STATUSCODE_SUCCESS,
				  "Your addresses are valid! Yahoo!",
				  this);
	this->Options.append( (Ptr*) ptrCode);
    }

    pkt = new char[this->getSize()];
    IsDone = false;
    this->MRT = 31;
    this->send();
}
	
// used as DECLINE reply
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgDecline> decline)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     decline->getIface(),decline->getAddr(), REPLY_MSG, decline->getTransID())
{
    this->copyRelayInfo((Ptr*)decline);

    SmartPtr<TOpt> ptrOpt;
    // include our DUID
    ptrOpt = decline->getOption(OPTION_SERVERID);
    Options.append(ptrOpt);

    // copy client's DUID
    SmartPtr<TSrvOptClientIdentifier> ptrClntDUID;
    ptrOpt = decline->getOption(OPTION_CLIENTID);
    ptrClntDUID = (Ptr*) ptrOpt;
    Options.append(ptrOpt);
    duidOpt=(Ptr*)ptrOpt;
    SmartPtr<TAddrClient> ptrClient = AddrMgr->getClient(duidOpt->getDUID());
    if (!ptrClient) {
	Log(Warning) << "Received DECLINE from unknown client, DUID=" << *duidOpt->getDUID() << LogEnd;
	IsDone = true;
	return;
    }

    SmartPtr<TDUID> declinedDUID = new TDUID("X",1);
    SmartPtr<TAddrClient> declinedClient = AddrMgr->getClient( declinedDUID );
    if (!declinedClient) {
	declinedClient = new TAddrClient( declinedDUID );
	AddrMgr->addClient(declinedClient);
    }

    decline->firstOption();
    while (ptrOpt = decline->getOption() ) 
    {
        switch (ptrOpt->getOptType()) 
	{
        case OPTION_IA: 
	{
	    SmartPtr<TSrvOptIA_NA> ptrIA_NA = (Ptr*) ptrOpt;
	    SmartPtr<TAddrIA> ptrIA = ptrClient->getIA(ptrIA_NA->getIAID());
	    if (!ptrIA) 
	    {
		Options.append( new TSrvOptIA_NA(ptrIA_NA->getIAID(), 0, 0, STATUSCODE_NOBINDING,
					     "No such IA is bound.",this) );
		continue;
	    }

	    // create empty IA
	    SmartPtr<TSrvOptIA_NA> replyIA_NA = new TSrvOptIA_NA(ptrIA_NA->getIAID(), 0, 0, this);
	    int AddrsDeclinedCnt = 0;

	    // IA found in DB, now move each addr in DB to "declined-client" and 
	    // ignore those, which are not present id DB
	    SmartPtr<TOpt> subOpt;
	    SmartPtr<TSrvOptIAAddress> addr;
	    SmartPtr<TAddrIA> declinedIA = declinedClient->getIA(0);
	    if (!declinedIA)
	    {
		declinedIA=new TAddrIA(decline->getIface(), SmartPtr<TIPv6Addr>(new TIPv6Addr()), declinedDUID, 0, 0, 0);
		declinedClient->addIA(declinedIA);
	    }
	    ptrOpt->firstOption();
	    while( subOpt = ptrOpt->getOption() ) {
		if (subOpt->getOptType()==OPTION_IAADDR)
		{
		    addr = (Ptr*) subOpt;
		    addr->setValid(0);
		    addr->setPref(0);
		    
		    // if there's no such address in out DB
		    if (!ptrIA->getAddr( addr->getAddr() ) )
			continue;
		    // add this addr to declined addrs...
		    declinedIA->addAddr( new TAddrAddr( addr->getAddr(),DECLINED_TIMEOUT,DECLINED_TIMEOUT) );
		    // ... and remove it from client's IA...
		    ptrIA->delAddr( addr->getAddr() );
		    // ... and finally append it in reply
		    replyIA_NA->addOption( subOpt );
		    AddrsDeclinedCnt++;
		};
	    }
	    Options.append((Ptr*)replyIA_NA);
	    char buf[10];
	    sprintf(buf,"%d",AddrsDeclinedCnt);
	    string tmp = buf;
	    SmartPtr<TSrvOptStatusCode> optStatusCode = 
		new TSrvOptStatusCode(STATUSCODE_SUCCESS,tmp+" addrs declined.",this);
	    replyIA_NA->addOption( (Ptr*) optStatusCode );
	};
	case OPTION_IA_TA:
	    Log(Info) << "TA address declined. Oh well. Since it's temporary, let's ignore it entirely." << LogEnd;
	    break;
	default:
	    break;
	}
    }

    pkt = new char[this->getSize()];
    IsDone = false;
    this->MRT = 31;
    this->send();
}

// used as REBIND reply
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgRebind> rebind)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     rebind->getIface(),rebind->getAddr(), REPLY_MSG, rebind->getTransID())
{
    this->copyRelayInfo((Ptr*)rebind);

    unsigned long addrCount=0;
    SmartPtr<TOpt> ptrOpt;

    setOptionsReqOptClntDUID((Ptr*)rebind);
    
    rebind->firstOption();
    while (ptrOpt = rebind->getOption() ) 
    {
        switch (ptrOpt->getOptType()) 
        {
        case OPTION_IA: 
            {
               SmartPtr<TSrvOptIA_NA> optIA_NA;
                optIA_NA = new TSrvOptIA_NA(CfgMgr, AddrMgr, (Ptr*)ptrOpt, 
                    rebind->getAddr(), duidOpt->getDUID(),
                    rebind->getIface(), addrCount, RENEW_MSG,
                    this);
		if (optIA_NA->getStatusCode() != STATUSCODE_NOBINDING )
		    this->Options.append((Ptr*)optIA_NA);
		else {
		    this->IsDone = true;
		    Log(Notice) << "REBIND received with unknown addresses and " 
			      << "was silently discarded." << LogEnd;
		    return;
		}
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
            appendDefaultOption(ptrOpt);
            break;
        }
    }
    appendRequestedOptions(duidOpt->getDUID(),rebind->getAddr(),rebind->getIface(),reqOpts);

    // append our DUID
    SmartPtr<TSrvOptServerIdentifier> srvDUID=new TSrvOptServerIdentifier(CfgMgr->getDUID(),this);
    this->Options.append((Ptr*)srvDUID);
    
    pkt = new char[this->getSize()];
    IsDone = false;
    this->MRT = 0;

    this->send();

}

// used as RELEASE replay
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgRelease> release)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     release->getIface(),release->getAddr(), REPLY_MSG, 
	     release->getTransID())
{
    this->copyRelayInfo((Ptr*)release);

     //FIXME:When the server receives a Release message via unicast from a client
    //to which the server has not sent a unicast option, the server
    //discards the Release message and responds with a Reply message
    //containing a Status Code option with value UseMulticast, a Server
    //Identifier option containing the server's DUID, the Client Identifier
    //option from the client message, and no other options.

    SmartPtr<TSrvOptClientIdentifier> clntID;
    SmartPtr<TOpt> opt, subOpt;

    release->firstOption();
    opt = release->getOption(OPTION_SERVERID);
    Options.append(opt);

    release->firstOption();
    opt = release->getOption(OPTION_CLIENTID);
    Options.append(opt);
    clntID=(Ptr*) opt;

    SmartPtr<TAddrClient> client = AddrMgr->getClient(clntID->getDUID());
    if (!client) {
	Log(Warning) << "Received RELEASE from unknown client." << LogEnd;
	IsDone = true;
	return;
    }

    release->firstOption();
    while(opt=release->getOption()) {
	switch (opt->getOptType()) {
	case OPTION_IA: {
	    SmartPtr<TSrvOptIA_NA> clntIA = (Ptr*) opt;
	    SmartPtr<TSrvOptIAAddress> addr;
	    bool anyDeleted=false;
	    
	    // does this client has IA? (iaid check)
	    SmartPtr<TAddrIA> ptrIA = client->getIA(clntIA->getIAID() );
	    if (!ptrIA) {
		Log(Warning) << "No such IA (iaid=" << clntIA->getIAID() << ") found for client:" << *clntID->getDUID() << LogEnd;
		Options.append( new TSrvOptIA_NA(clntIA->getIAID(), 0, 0, STATUSCODE_NOBINDING,"No such IA is bound.",this) );
		continue;
	    }
	    
	    // let's verify each address
	    clntIA->firstOption();
	    while(subOpt=clntIA->getOption()) {
		if (subOpt->getOptType()!=OPTION_IAADDR)
		    continue;
		addr = (Ptr*) subOpt;
		if (AddrMgr->delClntAddr(clntID->getDUID(), clntIA->getIAID(), addr->getAddr(), false) ) {
		    CfgMgr->delClntAddr(this->Iface,addr->getAddr());
		    anyDeleted=true;                    
		} else {
		    Log(Warning) << "No such binding found: client=" << clntID->getDUID()->getPlain() << ", IA (iaid=" 
				 << clntIA->getIAID() << "), addr="<< addr->getAddr()->getPlain() << LogEnd;
		};
	    };

	    // send result to the client
	    if (!anyDeleted)
	    {
		SmartPtr<TSrvOptIA_NA> ansIA(new TSrvOptIA_NA(clntIA->getIAID(), clntIA->getT1(),clntIA->getT2(),this));
		Options.append((Ptr*)ansIA);
		ansIA->addOption(new TSrvOptStatusCode(STATUSCODE_NOBINDING, "Not every address had binding.",this));
	    };
	    break;
	}
	case OPTION_IA_TA: {
	    SmartPtr<TSrvOptTA> ta = (Ptr*) opt;
	    bool anyDeleted = false;
	    SmartPtr<TAddrIA> ptrIA = client->getTA(ta->getIAID() );
	    if (!ptrIA) {
		Log(Warning) << "No such TA (iaid=" << ta->getIAID() << ") found for client:" << *clntID->getDUID() << LogEnd;
		Options.append( new TSrvOptTA(ta->getIAID(), STATUSCODE_NOBINDING, "No such IA is bound.", this) );
		continue;
	    }
	    
	    // let's verify each address
	    ta->firstOption();
	    while ( subOpt = (Ptr*) ta->getOption() ) {
		if (subOpt->getOptType()!=OPTION_IAADDR)
		    continue;
		SmartPtr<TSrvOptIAAddress> addr = (Ptr*) subOpt;
		if (AddrMgr->delTAAddr(clntID->getDUID(), ta->getIAID(), addr->getAddr()) ) {
		    CfgMgr->delTAAddr(this->Iface);
		    anyDeleted=true;                    
		} else {
		    Log(Warning) << "No such binding found: client=" << clntID->getDUID()->getPlain() << ", TA (iaid=" 
			     << ta->getIAID() << "), addr="<< addr->getAddr()->getPlain() << LogEnd;
		};
	    }
	    
	    // send results to the client
	    if (!anyDeleted)
	    {
		SmartPtr<TSrvOptTA> answerTA = new TSrvOptTA(ta->getIAID(), STATUSCODE_NOBINDING, "Not every address had binding.", this);
		Options.append((Ptr*)answerTA);
	    };
	    break;
	}
	default:
	    break;
	}; // switch(...)
    } // while 
    
    Options.append(new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					 "All IAs in RELEASE message were processed.",this));
    
    pkt = new char[this->getSize()];
    IsDone = false;

    this->MRT=46;
    this->send();
}

// used as RENEW reply
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
                           SmartPtr<TSrvTransMgr> transMgr, 
                           SmartPtr<TSrvCfgMgr> CfgMgr, 
                           SmartPtr<TSrvAddrMgr> AddrMgr,
                           SmartPtr<TSrvMsgRenew> renew)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     renew->getIface(),renew->getAddr(), REPLY_MSG, renew->getTransID())
{
    this->copyRelayInfo((Ptr*)renew);
    // uncomment this to test REBIND
    //IsDone = true;
    //return;
    // uncomment this to test REBIND

    unsigned long addrCount=0;
    SmartPtr<TOpt> ptrOpt;
    setOptionsReqOptClntDUID((Ptr*)renew);
    
    renew->firstOption();
    while (ptrOpt = renew->getOption() ) 
    {
        switch (ptrOpt->getOptType()) 
        {
        case OPTION_SERVERID:
            this->Options.append(ptrOpt);
            break;
        case OPTION_IA: {
	    SmartPtr<TSrvOptIA_NA> optIA_NA;
	    optIA_NA = new TSrvOptIA_NA(CfgMgr, AddrMgr, (Ptr*)ptrOpt, 
					renew->getAddr(), duidOpt->getDUID(),
					renew->getIface(), addrCount, RENEW_MSG, this);
	    this->Options.append((Ptr*)optIA_NA);
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
            appendDefaultOption(ptrOpt);
            break;
        }
    }
    appendRequestedOptions(duidOpt->getDUID(),renew->getAddr(),renew->getIface(),reqOpts);
    pkt = new char[this->getSize()];
    IsDone = false;
    this->MRT = 0;
    this->send();
}

/** 
 * this constructor is used to construct REPLY for REQUEST message
 * 
 * @param ifaceMgr 
 * @param transMgr 
 * @param CfgMgr 
 * @param AddrMgr 
 * @param request 
 */
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgRequest> request)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     request->getIface(),request->getAddr(), REPLY_MSG, request->getTransID())
{
    this->copyRelayInfo((Ptr*)request);

    SmartPtr<TOpt>       opt;
    SmartPtr<TSrvOptClientIdentifier> optClntID;
    SmartPtr<TDUID>      clntDuid;
    SmartPtr<TIPv6Addr>  clntAddr;
    unsigned int         clntIface;

    opt = request->getOption(OPTION_CLIENTID);
    optClntID = (Ptr*) opt;
    clntDuid  = optClntID->getDUID();
    clntAddr  = request->getAddr();
    clntIface = request->getIface();

    setOptionsReqOptClntDUID((Ptr*)request);

    // is this client supported?
    if (!CfgMgr->isClntSupported(clntDuid, clntAddr, clntIface)) {
        //No reply for this client 
	Log(Notice) << "Client (DUID=" << clntDuid->getPlain() << ",addr=" << *clntAddr 
		    << ") was rejected due to accept-only or reject-client." << LogEnd;
        IsDone=true;
        return;
    }

    // try to provide answer for all options
    request->firstOption();
    while ( opt = request->getOption() ) {
	switch (opt->getOptType()) {
	case OPTION_SERVERID    : {
	    this->Options.append(opt);
	    break;
	}
	case OPTION_IA          : {
	    SmartPtr<TSrvOptIA_NA> optIA;
	    optIA = new TSrvOptIA_NA(AddrMgr, CfgMgr, (Ptr*) opt, clntDuid, 
				     clntAddr, clntIface, REQUEST_MSG,this);
	    this->Options.append((Ptr*)optIA);
	    break;
	}
	case OPTION_IA_TA: {
	    SmartPtr<TSrvOptTA> ta;
	    ta = new TSrvOptTA(AddrMgr, CfgMgr, (Ptr*) opt, clntDuid, clntAddr,
			       clntIface, REQUEST_MSG, this);
	    this->Options.append((Ptr*)ta);
	    break;
	}
	case OPTION_STATUS_CODE :
	case OPTION_RELAY_MSG   :
	case OPTION_PREFERENCE  :
	case OPTION_RECONF_MSG  :
	case OPTION_INTERFACE_ID:
	case OPTION_IAADDR      :
	case OPTION_UNICAST     :
	case OPTION_RAPID_COMMIT:
	    Log(Warning) << "Invalid option " << opt->getOptType()<<" received." << LogEnd;
	    break;
	case OPTION_FQDN : {
	    SmartPtr<TSrvOptFQDN> requestFQDN = (Ptr*) opt;
	    SmartPtr<TSrvOptFQDN> optFQDN;
	    optFQDN = this->prepareFQDN(requestFQDN, clntDuid, clntAddr, true);
	    if (optFQDN) {
		this->Options.append((Ptr*) optFQDN);
	    }
	    break;
	}
	default:
	    appendDefaultOption(opt);
	    break;
	}
    }

    appendRequestedOptions(clntDuid, clntAddr, clntIface, reqOpts);
    pkt = new char[this->getSize()];
    IsDone = false;
    SmartPtr<TIPv6Addr> ptrAddr;
    this->MRT = 330; 
    this->send();
}

//SOLICIT
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgSolicit> solicit)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,solicit->getIface(),
	     solicit->getAddr(),REPLY_MSG,solicit->getTransID())
{
    this->copyRelayInfo((Ptr*)solicit);

    SmartPtr<TOpt>       opt;
    SmartPtr<TSrvOptClientIdentifier> optClntID;
    SmartPtr<TDUID>      clntDuid;
    SmartPtr<TIPv6Addr>  clntAddr;
    unsigned int         clntIface;

    opt = solicit->getOption(OPTION_CLIENTID);
    optClntID = (Ptr*) opt;
    clntDuid  = optClntID->getDUID();
    clntAddr  = solicit->getAddr();
    clntIface = solicit->getIface();

    setOptionsReqOptClntDUID((Ptr*)solicit);

    // include our DUID 
    SmartPtr<TSrvOptServerIdentifier> srvDUID=new TSrvOptServerIdentifier(CfgMgr->getDUID(),this);
    this->Options.append((Ptr*)srvDUID);

    // include rapid commit
    SmartPtr<TSrvOptRapidCommit> optRapidCommit = new TSrvOptRapidCommit(this);
    this->Options.append((Ptr*)optRapidCommit);

    // is this client supported?
    if (!CfgMgr->isClntSupported(clntDuid, clntAddr, clntIface)) {
        //No reply for this client 
	Log(Notice) << "Client (DUID=" << clntDuid->getPlain() << ",addr=" << *clntAddr 
		    << ") was rejected due to accept-only or reject-client." << LogEnd;
        IsDone=true;
        return;
    }

    //So if we can do something for this client at least set configuration
    //parameters - let's do  option by option - try to answer to it
    solicit->firstOption();
    while ( opt = solicit->getOption() ) 
    {
        switch (opt->getOptType()) 
        {
	case OPTION_SERVERID    :
	    Log(Warning) << "Solicit message contains ServerID option. Rejecting message. " << LogEnd;
	    IsDone = true;
	    return;
	case OPTION_IA          : {
	    SmartPtr<TSrvOptIA_NA> optIA_NA;
	    optIA_NA = new TSrvOptIA_NA(AddrMgr, CfgMgr, (Ptr*) opt, clntDuid, clntAddr, 
					clntIface, REQUEST_MSG,this);
	    this->Options.append((Ptr*)optIA_NA);
	    break;
	}
	case OPTION_IA_TA: {
	    SmartPtr<TSrvOptTA> ta;
	    ta = new TSrvOptTA(AddrMgr, CfgMgr, (Ptr*) opt, clntDuid, clntAddr,
			       clntIface, REQUEST_MSG, this);
	    this->Options.append((Ptr*)ta);
	    break;
	}
	case OPTION_STATUS_CODE : 
	case OPTION_RELAY_MSG   :
	case OPTION_PREFERENCE  :
	case OPTION_RECONF_MSG  :
	case OPTION_INTERFACE_ID:
	case OPTION_IAADDR      :
	case OPTION_UNICAST     :
	    Log(Warning) << "Invalid option type("
			 <<opt->getOptType()<<") received." << LogEnd;
	    break;
	case OPTION_RAPID_COMMIT:
	    Log(Info) << "SOLICIT with RAPID-COMMIT received." << LogEnd;
	    break;
	default:
	    appendDefaultOption(opt);
	    break;
        }
    }
    appendRequestedOptions(clntDuid, clntAddr, clntIface, reqOpts);

    pkt = new char[this->getSize()];
    IsDone = false;
    SmartPtr<TIPv6Addr> ptrAddr;
    this->MRT = 330; 
    this->send();
}

// INFORMATION-REQUEST answer
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgInfRequest> question)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,question->getIface(),
	     question->getAddr(),REPLY_MSG,question->getTransID())
{
    this->copyRelayInfo((Ptr*)question);

    SmartPtr<TOpt> ptrOpt;
    setOptionsReqOptClntDUID((Ptr*)question);
    
    Log(Debug) << "Received INF-REQUEST requesting " << this->showRequestedOptions(this->reqOpts) << "." << LogEnd;

    question->firstOption();
    while (ptrOpt = question->getOption() ) 
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
            case OPTION_IA          : 
            case OPTION_IA_TA       :
                Log(Warning) << "Invalid option " << ptrOpt->getOptType() <<" received." << LogEnd;
                break;
            default:
                this->appendDefaultOption(ptrOpt);
            break;
        }
    }
    if (appendRequestedOptions(duidOpt->getDUID(),question->getAddr(),question->getIface(),reqOpts)) {
        // include our DUID
        SmartPtr<TSrvOptServerIdentifier> srvDUID=new TSrvOptServerIdentifier(CfgMgr->getDUID(),this);
        this->Options.append((Ptr*)srvDUID);

        pkt = new char[this->getSize()];
        IsDone = false;
        this->MRT = 330; // FIXME: 
        this->send();
    } else {
	Log(Warning) << "No options to answer in INF-REQUEST, so REPLY will not be send." << LogEnd;
        IsDone=true;
    }
}

void TSrvMsgReply::doDuties() {
    IsDone = true;
}

unsigned long TSrvMsgReply::getTimeout() {
    unsigned long diff = now() - this->FirstTimeStamp;
    if (diff>SERVER_REPLY_CACHE_TIMEOUT)
	return 0;
    return SERVER_REPLY_CACHE_TIMEOUT-diff;
}

bool TSrvMsgReply::check() {
    return false;
}

void TSrvMsgReply::appendDefaultOption(SmartPtr<TOpt> ptrOpt) {
    int opt = ptrOpt->getOptType();
    switch(opt)
    {
    case OPTION_CLIENTID :
        this->Options.append(ptrOpt);
        break;
    case OPTION_ORO:
        //we found it at the beginning of function
        break;
    case OPTION_ELAPSED_TIME :
        //Ignore - BE PATIENT MY CLIENT!!!
        break;
    default:
	// all other options set in OPTION REQUEST
	if (!reqOpts->isOption(opt))
	    reqOpts->addOption(opt);
	break;
    }
}

/** 
 * finds CLIENT_DUID and OPTIONREQUEST in options
 * 
 * @param msg - parent message
 */
void TSrvMsgReply::setOptionsReqOptClntDUID(SmartPtr<TMsg> msg)
{
    reqOpts= SmartPtr<TSrvOptOptionRequest>();
   // remember client's DUID
    duidOpt = (Ptr*) msg->getOption(OPTION_CLIENTID);
    //remember requested option in order to add number of "hint" options
    //wich are included in packet but not in Request Option
    //if Request option wasn't included by client - create new one
    //in which number of "hint" options could be stored
    this->reqOpts=(Ptr*)msg->getOption(OPTION_ORO);
    //If there is no option Request it will ba added
    if (!reqOpts)
        this->reqOpts=new TSrvOptOptionRequest(this);
}

TSrvMsgReply::~TSrvMsgReply()
{

}

string TSrvMsgReply::getName() {
    return "REPLY";
}

/** 
 * This function creates FQDN option and executes DNS Update procedure (if necessary)
 * 
 * @param requestFQDN 
 * @param clntDuid 
 * @param clntAddr 
 * @param doRealUpdate - should the real update be performed (for example if response for
 *                       SOLICIT is prepare, this should be set to false)
 * 
 * @return 
 */
SPtr<TSrvOptFQDN> TSrvMsgReply::prepareFQDN(SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
					    SPtr<TIPv6Addr> clntAddr, bool doRealUpdate) {
    if (requestFQDN->getNFlag()) {
	Log(Notice) << "No DNS Update required by client." << LogEnd;
	return 0;
    }

    SmartPtr<TSrvOptFQDN> optFQDN;
    SmartPtr<TSrvCfgIface> ptrIface = SrvCfgMgr->getIfaceByID( this->Iface );
    if (!ptrIface) {
	Log(Crit) << "Msg received through not configured interface. "
	    "Somebody call an exorcist!" << LogEnd;
	this->IsDone = true;
	return 0;
    }
    // FQDN is chosen, "" if no name for this host is found.

    Log(Debug) << "Requesting FQDN for client with DUID=" << clntDuid->getPlain() << ", addr=" << clntAddr->getPlain() << LogEnd;
    SPtr<TFQDN> fqdn = ptrIface->getFQDNName(clntDuid,clntAddr);
    if (!fqdn) {
	Log(Debug) << "Unable to find FQDN for this client." << LogEnd;
	return 0;
    } 

    optFQDN = new TSrvOptFQDN(fqdn->getName(), this);
    // FIXME: For the moment, only the client make the DNS update
    optFQDN->setSFlag(false);
    // Setting the O Flag correctly according to the difference between O flags
    optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
    string fqdnName = fqdn->getName();
    
    fqdn->setUsed(true);
    int FQDNMode = ptrIface->getFQDNMode();
    Log(Debug) << "#### Adding FQDN Option in REPLY message: " << fqdnName << ", FQDNMode=" << FQDNMode << LogEnd;

    if ( FQDNMode==1 || FQDNMode==2 ) {
	Log(Debug) << "Server configuration allow DNS updates for " << clntDuid->getPlain() << LogEnd;
	
	if (FQDNMode == 1) optFQDN->setSFlag(false);
	else if (FQDNMode == 2)  optFQDN->setSFlag(true); // letting client update his AAAA
	// Setting the O Flag correctly according to the difference between O flags
	optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
	// Here we check if all parameters are set, and do the DNS update if possible
	List(TIPv6Addr) DNSSrvLst = *ptrIface->getDNSServerLst();
	//if ( DNSSrvLst.count() > 0 && fqdn.size() > 0) {
	Log(Debug) << "All parameters are set to perform DNS update.for client PTR. Selecting first DNS server" << LogEnd;
	SmartPtr<TIPv6Addr> DNSAddr;
	
	// For the moment, we just take the first DNS entry.
	DNSSrvLst.first();
	DNSAddr = DNSSrvLst.get();
	
	SmartPtr<TAddrClient> ptrAddrClient = SrvAddrMgr->getClient(clntDuid);	
	if (!ptrAddrClient) { 
	    Log(Warning) << "Unable to find client."; 
	    return 0;
	}
	ptrAddrClient->firstIA();
	SmartPtr<TAddrIA> ptrAddrIA = ptrAddrClient->getIA();
	if (!ptrAddrIA) { 
	    Log(Warning) << "Client does not have any addresses assigned." << LogEnd; 
	    return 0;
	}
	ptrAddrIA->firstAddr();
	SmartPtr<TAddrAddr> addr = ptrAddrIA->getAddr();
	SmartPtr<TIPv6Addr> IPv6Addr = addr->get();
	
	Log(Debug) << "Here I get : DNS server (" << *DNSAddr << "), IP (" << *IPv6Addr << ") and FQDN " 
		   << fqdnName << LogEnd;
	
	//Test for DNS update
	unsigned int dotpos = fqdnName.find(".");
	string hostname = "";
	string domain = "";
	if (dotpos == string::npos) {
	    Log(Warning) << "Name provided for DNS update s not a FQDN. [" << fqdnName
			 << "] Trying to do the  update..." << LogEnd;
	    hostname = fqdnName;
	}
	else {
	    hostname = fqdnName.substr(0, dotpos);
	    domain = fqdnName.substr(dotpos + 1, fqdnName.length() - dotpos - 1);
	}
	
#ifndef MOD_SRV_DISABLE_DNSUPDATE
	DnsUpdateResult result = DNSUPDATE_SKIP;
	DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), (char*) domain.c_str(), 
				       (char*) hostname.c_str(), IPv6Addr->getPlain(), "2h", FQDNMode);
	result = act->run();
	delete act;

	switch (result) {
	case DNSUPDATE_SUCCESS:
	    Log(Notice) << "FQDN Configured successfully !" << LogEnd;
	    break;
	case DNSUPDATE_ERROR:
	    Log(Warning) << "DNS Update failed." << LogEnd;
	    break;
	case DNSUPDATE_CONNFAIL:
	    Log(Warning) << "Unable to establish conntection to the DNS server." << LogEnd;
	    break;
	case DNSUPDATE_SRVNOTAUTH:
	    Log(Warning) << "DNS Update failed: no authorisation." << LogEnd;
	    break;
	case DNSUPDATE_SKIP:
	    Log(Notice) << "DNS Update was skipped." << LogEnd;
	    break;
	}
#else
	Log(Error) << "This server is compiled without DNS Update support." << LogEnd;
#endif

    } else {
	Log(Debug) << "Server configuration does NOT allow DNS updates for " << *clntDuid << LogEnd;
	optFQDN->setNFlag(true);
    }
    
    return optFQDN;
}
