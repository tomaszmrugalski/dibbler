/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgReply.cpp,v 1.8 2004-06-20 21:00:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 * Revision 1.6  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 * Revision 1.5  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *                                                                           
 */

#include "SmartPtr.h"
#include "SrvMsgReply.h"
#include "SrvMsg.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptStatusCode.h"
#include "SrvOptIAAddress.h"
#include "SrvOptIA_NA.h"
#include "SrvOptRapidCommit.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptTimeZone.h"
#include "SrvOptDomainName.h"
#include "SrvOptDNSServers.h"
#include "SrvOptNTPServers.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "Logger.h"
#include "OptStatusCode.h"
#include <cmath>


// used ad CONFIRM reply
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgConfirm> confirm)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     confirm->getIface(),confirm->getAddr(), REPLY_MSG, 
	     confirm->getTransID())
{
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
    bool NotOnLink;
    while (ptrOpt=confirm->getOption() && (!NotOnLink) ) {
	switch (ptrOpt->getOptType()) {
	case OPTION_IA: 
	{
	    SmartPtr<TSrvOptIA_NA> ptrIA = (Ptr*) ptrOpt;
	    SmartPtr<TOpt> ptrSubOpt;
	    ptrIA->firstOption();
	    while ( ptrSubOpt = ptrIA->getOption() && (!NotOnLink) ) {
		if (ptrSubOpt->getOptType() == OPTION_IAADDR) {
		    SmartPtr< TSrvOptIAAddress > ptrAddr = (Ptr*) ptrSubOpt;
		    // addr found. Now look for class it belongs to
		    SmartPtr<TSrvCfgAddrClass> ptrClass;
		    ptrIface->firstAddrClass();
		    NotOnLink = true;
		    while ( ptrClass = ptrIface->getAddrClass() && (!NotOnLink) ) {
			if (ptrClass->addrInPool(ptrAddr->getAddr() ) )
			    NotOnLink = false;
			
		    }
		}
	    }
	    break;
	}   
	default:
	    // other options - ignore them
	    continue;
	}
    }
    if (NotOnLink) {
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
	std::clog << logger::logWarning << "Received DECLINE from unknown client:" 
		  << *duidOpt->getDUID() << logger::endl;
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
		    std::clog << logger::logNotice << "REBIND received with unknown addresses and " 
			      << "was silently discarded." << logger::endl;
		    return;
		}
                break;
            }
        case OPTION_IAADDR:
        case OPTION_RAPID_COMMIT:
        case OPTION_STATUS_CODE:
        case OPTION_PREFERENCE:
        case OPTION_UNICAST:
            std::clog << "Invalid option (" <<ptrOpt->getOptType() << ") received." << logger::endl;
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
     //FIXME:When the server receives a Release message via unicast from a client
    //to which the server has not sent a unicast option, the server
    //discards the Release message and responds with a Reply message
    //containing a Status Code option with value UseMulticast, a Server
    //Identifier option containing the server's DUID, the Client Identifier
    //option from the client message, and no other options.

    SmartPtr<TSrvOptIA_NA> clntIA;
    SmartPtr<TSrvOptClientIdentifier> clntID;
    SmartPtr<TOpt> opt;

    release->firstOption();
    opt = release->getOption(OPTION_SERVERID);
    Options.append(opt);

    release->firstOption();
    opt = release->getOption(OPTION_CLIENTID);
    Options.append(opt);
    clntID=(Ptr*) opt;

    release->firstOption();
    while(opt=release->getOption()) {
        if (opt->getOptType()!=OPTION_IA)
	    continue;

	clntIA=(Ptr*) opt;
	SmartPtr<TSrvOptIAAddress> addr;
	clntIA->firstOption();
	SmartPtr<TOpt> subOpt;
	bool anyDeleted=false;
	clntIA->firstOption();
	while(subOpt=clntIA->getOption()) {
	    if (subOpt->getOptType()!=OPTION_IAADDR)
		continue;
	    
	    addr=(Ptr*) subOpt;                
	    SmartPtr<TAddrClient> ptrClient = AddrMgr->getClient(clntID->getDUID());
	    if (!ptrClient) {
		std::clog << logger::logWarning << "Received RELEASE from unknown client." << logger::endl;
		IsDone = true;
		return;
	    }
	    SmartPtr<TAddrIA> ptrIA = ptrClient->getIA(clntIA->getIAID() );
	    if (!ptrIA) {
		std::clog << logger::logWarning << "No such IA(" << clntIA->getIAID() 
			  << ") found for client:" << *clntID->getDUID() << logger::endl;
		Options.append( new TSrvOptIA_NA(clntIA->getIAID(), 0, 0, STATUSCODE_NOBINDING,
						 "No such IA is bound.",this) );
	    } else {
		if (AddrMgr->delClntAddr(clntID->getDUID(),
					 clntIA->getIAID(),
					 addr->getAddr(),
					 false) )
		    anyDeleted=true;                    
		else
		{
		    Log(Warning) << "No such binding found: "
				 << "IAID:" <<clntIA->getIAID() 
				 << "Address:"<< addr->getAddr()->getPlain()
				 << "DUID:"<< clntID->getDUID()->getPlain()
				 << LogEnd;
		};
	    };
	};
	//After all the addresses have been processed, the server generates a
	//Reply message and includes a Status Code option with value Success,a
	//Server Identifier option with the server's DUID, and a Client
	//Identifier option with the client's DUID.  For each IA in the Release
	//message for which the server has no binding information, the server
	//adds an IA option using the IAID from the Release message, and
	//includes a Status Code option with the value NoBinding in the IA
	//option.  No other options are included in the IA option.
	if (!anyDeleted)
	{
	    SmartPtr<TSrvOptIA_NA> ansIA(new TSrvOptIA_NA(clntIA->getIAID(),
							  clntIA->getT1(),clntIA->getT2(),this));
	    Options.append((Ptr*)ansIA);
	    ansIA->addOption(new TSrvOptStatusCode(STATUSCODE_NOBINDING,
						   "Not every address had binding",this));
	};
    };
    
    Options.append(new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					 "All IA's in release message were processed",this));
    
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
        case OPTION_IA: 
            {
                SmartPtr<TSrvOptIA_NA> optIA_NA;
                optIA_NA = new TSrvOptIA_NA(CfgMgr, AddrMgr, (Ptr*)ptrOpt, 
                    renew->getAddr(), duidOpt->getDUID(),
                    renew->getIface(), addrCount, RENEW_MSG, this);
                this->Options.append((Ptr*)optIA_NA);
            }
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
            std::clog << "Invalid option type("<<ptrOpt->getOptType()<<") received." << logger::endl;
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

// used as REQUEST reply
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgRequest> request)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,
	     request->getIface(),request->getAddr(), REPLY_MSG, request->getTransID())
{
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
	Log(Notice) << "Client with DUID=" << clntDuid << "/addr=" << clntAddr 
		    << " was rejected (due to accept-only or reject-client)." << LogEnd;
        IsDone=true;
        return;
    }

    //So if we can do something for this client at least set configuration
    //parameters - let's do  option by option - try to answer to it
    request->firstOption();
    while ( opt = request->getOption() ) 
    {
        switch (opt->getOptType()) 
        {
	case OPTION_SERVERID    :
	    this->Options.append(opt);
	    break;
	case OPTION_IA          : 
	{
	    SmartPtr<TSrvOptIA_NA> optIA_NA;
	    optIA_NA=new TSrvOptIA_NA(AddrMgr, CfgMgr, (Ptr*) opt, 
				      clntDuid, clntAddr,
				      clntIface, REQUEST_MSG,this);
	    this->Options.append((Ptr*)optIA_NA);
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
	    std::clog << "Invalid option type("
		      <<opt->getOptType()<<") received." << logger::endl;
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

//SOLICIT
TSrvMsgReply::TSrvMsgReply(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
			   SmartPtr<TSrvTransMgr> transMgr, 
			   SmartPtr<TSrvCfgMgr> CfgMgr, 
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   SmartPtr<TSrvMsgSolicit> solicit)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,solicit->getIface(),
	     solicit->getAddr(),REPLY_MSG,solicit->getTransID())
{
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

    // is this client supported?
    if (!CfgMgr->isClntSupported(clntDuid, clntAddr, clntIface)) {
        //No reply for this client 
	Log(Notice) << "Client with DUID=" << clntDuid << "/addr=" << clntAddr 
		    << " was rejected (due to accept-only or reject-client)." << LogEnd;
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
	    this->Options.append(opt);
	    break;
	case OPTION_IA          : 
	{
	    SmartPtr<TSrvOptIA_NA> optIA_NA;
	    optIA_NA=new TSrvOptIA_NA(AddrMgr, CfgMgr, (Ptr*) opt, 
				      clntDuid, clntAddr, 
				      clntIface, REQUEST_MSG,this);
	    this->Options.append((Ptr*)optIA_NA);
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
	    std::clog << "Invalid option type("
		      <<opt->getOptType()<<") received." << logger::endl;
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

//Information request answer
TSrvMsgReply::TSrvMsgReply(
    SmartPtr<TSrvIfaceMgr> ifaceMgr, 
    SmartPtr<TSrvTransMgr> transMgr, 
    SmartPtr<TSrvCfgMgr> CfgMgr, 
    SmartPtr<TSrvAddrMgr> AddrMgr,
    SmartPtr<TSrvMsgInfRequest> question)
    :TSrvMsg(ifaceMgr,transMgr,CfgMgr,AddrMgr,question->getIface(),
                question->getAddr(),REPLY_MSG,question->getTransID())
{
    SmartPtr<TOpt> ptrOpt;
    setOptionsReqOptClntDUID((Ptr*)question);

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
                std::clog << "Invalid option type("
                    <<ptrOpt->getOptType() <<") received." << logger::endl;
                break;
            default:
                this->appendDefaultOption(ptrOpt);
            break;
        }
    }
    if (appendRequestedOptions(duidOpt->getDUID(),question->getAddr(),question->getIface(),reqOpts))
    {
        // include our DUID
        SmartPtr<TSrvOptServerIdentifier> srvDUID=new TSrvOptServerIdentifier(CfgMgr->getDUID(),this);
        this->Options.append((Ptr*)srvDUID);

        pkt = new char[this->getSize()];
        IsDone = false;
        //SmartPtr<TIPv6Addr> ptrAddr;
        this->MRT = 330; // FIXME: 
        this->send();
    }
    else
    {
        IsDone=false;
    }
}

void TSrvMsgReply::answer(SmartPtr<TMsg> Rep)
{
    // this should never happen
}

void TSrvMsgReply::doDuties()
{
    IsDone = true;
}

unsigned long TSrvMsgReply::getTimeout()
{
    return 0;
}

bool TSrvMsgReply::check()
{
    return false;
}

//Methods add standard options to options:
//OPTION_CLIENTID, OPTION_ORO, OPTION_ELAPSED_TIME amd
//OPTION_DNS_RESOLVERS, OPTION_DOMAIN_LIST, OPTION_NTP_SERVERS, OPTION_TIME_ZONE
//It requires to set reqOpt and for all the rest, which don't match these
//options above warning that option is not supported is logged
//ATTENTION: in order this method work field reqOpts in this class should be set
void TSrvMsgReply::appendDefaultOption(SmartPtr<TOpt> ptrOpt)
{
    switch(ptrOpt->getOptType())
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
        //add options requested by client to option Request Option if
        //client didn't included them
    case OPTION_DNS_RESOLVERS:
        if (!reqOpts->isOption(OPTION_DNS_RESOLVERS))
            reqOpts->addOption(OPTION_DNS_RESOLVERS);
        break;
    case OPTION_DOMAIN_LIST:
        if (!reqOpts->isOption(OPTION_DOMAIN_LIST))
            reqOpts->addOption(OPTION_DOMAIN_LIST);
        break;
    case OPTION_NTP_SERVERS:
        if (!reqOpts->isOption(OPTION_NTP_SERVERS))
            reqOpts->addOption(OPTION_NTP_SERVERS);
        break;
    case OPTION_TIME_ZONE:
        if (!reqOpts->isOption(OPTION_TIME_ZONE))
            reqOpts->addOption(OPTION_TIME_ZONE);
        break;
    default:
        std::clog << logger::logWarning << "Option not supported: type(" 
            << ptrOpt->getOptType() <<")."<< logger::endl;
        break;
    }
}

// finds CLIENT_DUID and OPTIONREQUEST in options
void TSrvMsgReply::setOptionsReqOptClntDUID(SmartPtr<TMsg> msg)
{
    reqOpts= SmartPtr<TSrvOptOptionRequest>();
   // remember client's DUID
    duidOpt = (Ptr*) msg->getOption(OPTION_CLIENTID);
    //remember requested option in order to add number of "hint" options
    //wich are included in packet but not in Request Option
    //if Request option wasn't included by client - create new one
    //in which number of "hint" options could be stored
    reqOpts=(Ptr*)msg->getOption(OPTION_ORO);
    //If there is no option Request it will ba added
    if (!reqOpts)
        reqOpts=new TSrvOptOptionRequest(this);
}

TSrvMsgReply::~TSrvMsgReply()
{

}

string TSrvMsgReply::getName() {
    return "REPLY";
}
