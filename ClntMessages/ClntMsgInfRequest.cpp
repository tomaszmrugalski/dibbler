/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgInfRequest.cpp,v 1.15 2008-08-29 00:07:28 thomson Exp $
 *
 */

#include "ClntMsgInfRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "Container.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgAdvertise.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptIA_NA.h"
#include "ClntOptElapsed.h"
#include "Logger.h"
#include "ClntOptOptionRequest.h"
#include "ClntCfgIface.h"
#include "ClntOptDNSServers.h"
#include "ClntOptNTPServers.h"
#include "ClntOptDomainName.h"
#include "ClntOptTimeZone.h"
#include <cmath>

TClntMsgInfRequest::TClntMsgInfRequest(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				       SmartPtr<TClntTransMgr> TransMgr,
				       SmartPtr<TClntCfgMgr>   CfgMgr, 
				       SmartPtr<TClntAddrMgr>  AddrMgr, 
				       SmartPtr<TClntCfgIface> iface)
    :TClntMsg(IfaceMgr, TransMgr, CfgMgr, AddrMgr, iface->getID(), SmartPtr<TIPv6Addr>() /*NULL*/, 
	      INFORMATION_REQUEST_MSG) {

    IRT = INF_TIMEOUT;
    MRT = INF_MAX_RT;
    MRC = 0;
    MRD = 0;
    RT= 0 ;
    
    Iface=iface->getID();
    IsDone=false;

    if (!CfgMgr->anonInfRequest()) {
	Options.append(new TClntOptClientIdentifier(ClntCfgMgr->getDUID(),this));
    } else {
	Log(Info) << "Sending anonymous INF-REQUEST (ClientID not included)." << LogEnd;
    }

    this->appendRequestedOptions();

    appendAuthenticationOption(AddrMgr);

    pkt = new char[getSize()];
}

//opts - all options list WITHOUT serverDUID including server id
TClntMsgInfRequest::TClntMsgInfRequest(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				       SmartPtr<TClntTransMgr> TransMgr,
				       SmartPtr<TClntCfgMgr>   CfgMgr, 
				       SmartPtr<TClntAddrMgr> AddrMgr, 
				       TContainer< SmartPtr<TOpt> > ReqOpts,
				       int iface)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,SmartPtr<TIPv6Addr>() /*NULL*/,
	      INFORMATION_REQUEST_MSG) {
    IRT = INF_TIMEOUT;
    MRT = INF_MAX_RT;
    MRC = 0;
    MRD = 0;
    RT=0;

    Iface=iface;
    IsDone=false;
    
    SmartPtr<TIfaceIface> ptrIface = IfaceMgr->getIfaceByID(iface);
    if (!ptrIface) {
	Log(Error) << "Unable to find interface with ifindex=" << iface 
		   << " while trying to generate INF-REQUEST." << LogEnd;
	this->IsDone = true;
	return;
    }
    Log(Debug) << "Creating INF-REQUEST on the " << ptrIface->getName()
	       << "/" << iface << "." << LogEnd;
    
    // copy whole list from Verify ...
    Options = ReqOpts;
    
    SmartPtr<TOpt> opt;
    Options.first();
    while(opt=Options.get())
    {
        switch (opt->getOptType())
        {       
            //These options possibly receipt from verify transaction
            //can't appear in request message and have to be deleted
            case OPTION_UNICAST:
            case OPTION_STATUS_CODE:
            case OPTION_PREFERENCE:
            case OPTION_IA_TA:
            case OPTION_RELAY_MSG:
            case OPTION_SERVERID:
            case OPTION_IA_NA:
            case OPTION_IAADDR:
            case OPTION_RAPID_COMMIT:
            case OPTION_INTERFACE_ID:
            case OPTION_RECONF_MSG:
            case OPTION_AUTH:
            case OPTION_AAAAUTH:
            case OPTION_KEYGEN:
                Options.del();
                break;        
        }
        //The other options can be included in Information request option
        //CLIENTID,ORO,ELAPSED_TIME,USER_CLASS,VENDOR_CLASS,
        //VENDOR_OPTS,DNS_RESOLVERS,DOMAIN_LIST,NTP_SERVERS,TIME_ZONE,
        //RECONF_ACCEPT - maybe also SERVERID if information request
        //is answer to reconfigure message
    }

    appendAuthenticationOption(AddrMgr);

    pkt = new char[getSize()];
}


void TClntMsgInfRequest::answer(SmartPtr<TClntMsg> msg)
{
    //server DUID from which there is answer
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) msg->getOption(OPTION_SERVERID);
    //which option have we requested from server
    SmartPtr<TClntOptOptionRequest> ptrORO;
    ptrORO = (Ptr*)getOption(OPTION_ORO);
    
    SmartPtr<TOpt> option;
    msg->firstOption();
    while(option = msg->getOption())
    {
        //if option did what it was supposed to do ???
	if (!option->doDuties()) {
	    // Log(Debug) << "Setting option " << option->getOptType() << " failed." << LogEnd;
	    continue;
	}
	if ( ptrORO && (ptrORO->isOption(option->getOptType())) )
	    ptrORO->delOption(option->getOptType());
	SmartPtr<TOpt> requestOpt;
	this->firstOption();
	while ( requestOpt = this->getOption()) 
	{
	    if (requestOpt->getOptType()==option->getOptType())
		this->Options.del();
	}//while
    }

    ptrORO->delOption(OPTION_INFORMATION_REFRESH_TIME);
    if (ptrORO && ptrORO->count())
    {
	if (ClntCfgMgr->insistMode()){ 
	    Log(Notice) << "Insist-mode enabled. Not all options were assigned (";
	    for (int i=0; i<ptrORO->count(); i++)
		Log(Cont) << ptrORO->getReqOpt(i) << " ";
	    Log(Cont) << "). Sending new INFORMATION-REQUEST." << LogEnd;
	    ClntTransMgr->sendInfRequest(Options,Iface);
	} else {
	    Log(Notice) << "Insist-mode disabled. Not all options were assigned (";
	    for (int i=0; i<ptrORO->count(); i++)
		Log(Cont) << ptrORO->getReqOpt(i) << " ";
	    Log(Cont) << "). They will remain unconfigured." << LogEnd;
	    IsDone = true;
	}
    } else {
	Log(Debug) << "All requested options were assigned." << LogEnd;
        IsDone=true;
    }
    return;
}

void TClntMsgInfRequest::doDuties()
{
    //timeout is reached, so let's retrasmit this message
    send();
    return;
}

bool TClntMsgInfRequest::check()
{
    return false;
}

string TClntMsgInfRequest::getName() {
    return "INF-REQUEST";
}

TClntMsgInfRequest::~TClntMsgInfRequest()
{
    delete [] pkt;
    pkt=NULL;
}
