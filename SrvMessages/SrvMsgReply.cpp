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
	     confirm->getIface(),confirm->getAddr(), REPLY_MSG, confirm->getTransID())
{
    SmartPtr<TSrvCfgIface> ptrIface = CfgMgr->getIface( confirm->getIface() );
    if (!ptrIface) {
	std::clog << logger::logCrit << "Msg received through not configured interface. "
	    "Somebody call an exorcist!" << logger::endl;
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
	     release->getIface(),release->getAddr(), REPLY_MSG, release->getTransID())
{
     //FIXME:When the server receives a Release message via unicast from a client
    //to which the server has not sent a unicast option, the server
    //discards the Release message and responds with a Reply message
    //containing a Status Code option with value UseMulticast, a Server
    //Identifier option containing the server's DUID, the Client Identifier
    //option from the client message, and no other options.


    //Upon the receipt of a valid Release message, the server examines the
    //IAs and the addresses in the IAs for validity.  If the IAs in the
    //message are in a binding for the client, and the addresses in the IAs
    //have been assigned by the server to those IAs, the server deletes the
    //addresses from the IAs and makes the addresses available for
    //assignment to other clients.  The server ignores addresses not
    //assigned to the IA, although it may choose to log an error.
    SmartPtr<TSrvOptIA_NA> clntIA;
    SmartPtr<TSrvOptClientIdentifier> clntID;
    SmartPtr<TOpt> opt;

    release->firstOption();
    while((opt=release->getOption())&&(opt->getOptType()!=OPTION_SERVERID)) ;    
    Options.append(opt);

    release->firstOption();
    while((opt=release->getOption())&&(opt->getOptType()!=OPTION_CLIENTID)) ;
    Options.append(opt);
    clntID=(Ptr*) opt;

    release->firstOption();
    while(opt=release->getOption())
        if (opt->getOptType()==OPTION_IA)
        {
            clntIA=(Ptr*) opt;
            SmartPtr<TSrvOptIAAddress> addr;
            clntIA->firstOption();
            SmartPtr<TOpt> subOpt;
            bool anyDeleted=false;
	    clntIA->firstOption();
            while(subOpt=clntIA->getOption())
                if (subOpt->getOptType()==OPTION_IAADDR)
                {
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
			if (!ptrIA->delAddr(addr->getAddr()))
			    anyDeleted=true;                    
			else
			{
			    clog<<logger::logWarning<<"No such binding found: "
				<< "IAID:"<<clntIA->getIAID()<< "Address:"<< addr->getAddr()->getPlain();
			    clog<<"DUID:"<<*clntID->getDUID();
			    clog << logger::endl;
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
    unsigned long addrCount=0;
    SmartPtr<TOpt> ptrOpt;
    
    setOptionsReqOptClntDUID((Ptr*)request);

    //Lists: of already assigned addresses to client - 
    SmartPtr<TContainer<SmartPtr<TIPv6Addr > > >
        assAddrLst(new TContainer<SmartPtr<TIPv6Addr> > ());
    //of classes from which addresses can be assigned (limits are kept)
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntClasses;
    //of all classes, from which client can be assigned addresses
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntAllClasses;
    //all passed to IA_NA option
    long *clntFreeAddr;
    long totalFreeAddresses;
    getFreeAddressesForClient(clntAllClasses,clntClasses,clntFreeAddr,
        totalFreeAddresses,duidOpt->getDUID(),request->getAddr(),
        request->getIface());
    //if client should be ignored - it is not supported
    if(!clntAllClasses->count())
    {
        //There is no reply for this client there is no classes
        //from which addresses/parameters can be assigned to it
        //the client tries to deceive it requests addresses
        //or information, while we not authorized to answer it
        IsDone=true;
        return;
    }   
    //So if we can do something for this client at least set configuration
    //parameters - let's do  option by option - try to answer to it
    request->firstOption();
    while (ptrOpt = request->getOption() ) 
    {
        switch (ptrOpt->getOptType()) 
        {
	case OPTION_SERVERID    :
	    this->Options.append(ptrOpt);
	    break;
	case OPTION_IA          : 
	{
	    char ignoreme;
	    SmartPtr<TSrvOptIA_NA> optIA_NA;
	    optIA_NA=new TSrvOptIA_NA(AddrMgr, clntClasses, clntFreeAddr, 
				      totalFreeAddresses, assAddrLst, (Ptr*)ptrOpt, ignoreme,
				      duidOpt->getDUID(),request->getAddr(), request->getIface(),
				      REQUEST_MSG,this);
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
		      <<ptrOpt->getOptType()<<") received." << logger::endl;
	    break;
            
            // not supported yet
	default:
	    appendDefaultOption(ptrOpt);
	    break;
        }
    }
    appendRequestedOptions(duidOpt->getDUID(),request->getAddr(),request->getIface(),reqOpts);
    delete clntFreeAddr;
    pkt = new char[this->getSize()];
    IsDone = false;
    SmartPtr<TIPv6Addr> ptrAddr;
    //assAddrLst->first(); - I wanted assign addresses here but it's done in IA_NA option
    this->MRT = 330; // FIXME: 
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
    unsigned long addrCount=0;
    SmartPtr<TOpt> ptrOpt;
    setOptionsReqOptClntDUID((Ptr*)solicit);
    
    SmartPtr<TContainer<SmartPtr<TIPv6Addr > > >
        assAddrLst(new TContainer<SmartPtr<TIPv6Addr> > ());
    
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntClasses;
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntAllClasses;

    long *clntFreeAddr;
    long totalFreeAddresses;
    getFreeAddressesForClient(clntAllClasses,clntClasses,clntFreeAddr,
        totalFreeAddresses,duidOpt->getDUID(),solicit->getAddr(),
        solicit->getIface(),true);

    if(!clntAllClasses->count())
    {
        //There is no reply for this client there is no classes
        //from which addresses/parameters can be assigned to it
        //the client tries to deceive it requests addresses
        //or information, while we not authorized to answer it
        IsDone=true;
        return;
    }
    
    //So if we can do something for this client at least set configuration
    //parameters - let's do it
    //Option by option - answer to it
    solicit->firstOption();
    while (ptrOpt = solicit->getOption() ) 
    {
        switch (ptrOpt->getOptType()) 
        {
            case OPTION_IA       : 
            {
                //if it's possible assign these addresses - so we use constructor
                //with request option
		char ignoreme;
                SmartPtr<TSrvOptIA_NA> optIA_NA;
                optIA_NA=new TSrvOptIA_NA(
                    AddrMgr, clntClasses, clntFreeAddr, 
                    totalFreeAddresses, assAddrLst, (Ptr*)ptrOpt, ignoreme,
                    duidOpt->getDUID(),solicit->getAddr(), solicit->getIface(),
                    REQUEST_MSG,this);
                    //optIA_NA = new TSrvOptIA_NA(CfgMgr,AddrMgr,
                    //          (Ptr*)ptrOpt,request->getAddr(),duidOpt->getDUID(),
                    //			request->getIface(),addrCount,REQUEST_MSG);
                    this->Options.append((Ptr*)optIA_NA);
                break;
            }
            case OPTION_RAPID_COMMIT:
            {
                SmartPtr<TSrvOptRapidCommit> optRap=new TSrvOptRapidCommit(this);
                this->Options.append((Ptr*) optRap);
                break;
            }
            case OPTION_STATUS_CODE : 
            case OPTION_SERVERID    :
            case OPTION_PREFERENCE  :
            case OPTION_UNICAST     :
            case OPTION_RELAY_MSG   :
            case OPTION_INTERFACE_ID:
            case OPTION_RECONF_MSG  :
            case OPTION_IAADDR      :
                std::clog << "Invalid option type("
                    << ptrOpt->getOptType() <<") received." << logger::endl;
                break;
            
            default:
                appendDefaultOption(ptrOpt);
            break;
        }
    }    
    appendRequestedOptions(duidOpt->getDUID(),solicit->getAddr(),solicit->getIface(),reqOpts);
    // include our DUID
    SmartPtr<TSrvOptServerIdentifier> srvDUID=new TSrvOptServerIdentifier(CfgMgr->getDUID(),this);
    this->Options.append((Ptr*)srvDUID);

    delete clntFreeAddr;
    pkt = new char[this->getSize()];
    IsDone = false;
    SmartPtr<TIPv6Addr> ptrAddr;
    assAddrLst->first();
    this->MRT = 330; // FIXME: 
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
