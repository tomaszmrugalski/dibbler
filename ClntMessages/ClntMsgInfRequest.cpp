/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgInfRequest.cpp,v 1.16 2009-03-24 23:17:17 thomson Exp $
 *
 */

#include "ClntMsgInfRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "Container.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgAdvertise.h"
#include "OptDUID.h"
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

TClntMsgInfRequest::TClntMsgInfRequest(SPtr<TClntCfgIface> iface)
    :TClntMsg(iface->getID(), 0, INFORMATION_REQUEST_MSG) {

    IRT = INF_TIMEOUT;
    MRT = INF_MAX_RT;
    MRC = 0;
    MRD = 0;
    RT= 0 ;
    
    Iface=iface->getID();
    IsDone=false;

    if (!ClntCfgMgr().anonInfRequest()) {
        Options.push_back(new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(), this));
    } else {
        Log(Info) << "Sending anonymous INF-REQUEST (ClientID not included)." << LogEnd;
    }

    this->appendRequestedOptions();

    appendAuthenticationOption();
    appendElapsedOption();
    send();
}

//opts - all options list WITHOUT serverDUID including server id
TClntMsgInfRequest::TClntMsgInfRequest(TOptList ReqOpts,
				       int iface)
    :TClntMsg(iface, 0, INFORMATION_REQUEST_MSG) {
    IRT = INF_TIMEOUT;
    MRT = INF_MAX_RT;
    MRC = 0;
    MRD = 0;
    RT=0;

    Iface=iface;
    IsDone=false;
    
    SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(iface);
    if (!ptrIface) {
        Log(Error) << "Unable to find interface with ifindex=" << iface 
                   << " while trying to generate INF-REQUEST." << LogEnd;
        IsDone = true;
        return;
    }
    Log(Debug) << "Creating INF-REQUEST on the " << ptrIface->getFullName()
	           << "." << LogEnd;
    
    // copy whole list from Verify ...
    Options = ReqOpts;
    
    SPtr<TOpt> opt;
    firstOption();
    while(opt=getOption())
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
	    case OPTION_ELAPSED_TIME:       //delete the old elapsed option,as we will append a new one
                delOption(opt->getOptType());
                break;        
        }
        //The other options can be included in Information request option
        //CLIENTID,ORO,ELAPSED_TIME,USER_CLASS,VENDOR_CLASS,
        //VENDOR_OPTS,DNS_RESOLVERS,DOMAIN_LIST,NTP_SERVERS,TIME_ZONE,
        //RECONF_ACCEPT - maybe also SERVERID if information request
        //is answer to reconfigure message
    }
    appendElapsedOption();
    appendAuthenticationOption();

    this->send();
}


void TClntMsgInfRequest::answer(SPtr<TClntMsg> msg)
{
    //which option have we requested from server
    SPtr<TClntOptOptionRequest> ptrORO;
    ptrORO = (Ptr*)getOption(OPTION_ORO);
    
    SPtr<TOpt> option;
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
	SPtr<TOpt> requestOpt;
	this->firstOption();
	while ( requestOpt = this->getOption()) 
	{
	    if (requestOpt->getOptType()==option->getOptType()) {
		delOption(requestOpt->getOptType());
		break;
	    }
	}//while
    }
    
    ptrORO->delOption(OPTION_INFORMATION_REFRESH_TIME);
    if (ptrORO && ptrORO->count())
    {
	if (ClntCfgMgr().insistMode()){ 
	    Log(Notice) << "Insist-mode enabled. Not all options were assigned (";
	    for (int i=0; i<ptrORO->count(); i++)
                Log(Cont) << ptrORO->getReqOpt(i) << " ";
	    Log(Cont) << "). Sending new INFORMATION-REQUEST." << LogEnd;
	    ClntTransMgr().sendInfRequest(Options,Iface);
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

TClntMsgInfRequest::~TClntMsgInfRequest() {
}
