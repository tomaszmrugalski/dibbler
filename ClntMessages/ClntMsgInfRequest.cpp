/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgInfRequest.cpp,v 1.2 2004-06-20 17:51:48 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "ClntMsgInfRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "Container.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgAdvertise.h"
#include "ClntOptServerIdentifier.h"
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

TClntMsgInfRequest::TClntMsgInfRequest(SmartPtr<TClntIfaceMgr> IfMgr, 
				       SmartPtr<TClntTransMgr> TransMgr,
				       SmartPtr<TClntCfgMgr>   ConfMgr, 
				       SmartPtr<TClntAddrMgr>  AddrMgr, 
				       SmartPtr<TClntCfgIface> iface)
    :TClntMsg(IfMgr, TransMgr, ConfMgr, AddrMgr, iface->getID(), SmartPtr<TIPv6Addr>() /*NULL*/, 
	      INFORMATION_REQUEST_MSG) {

    IRT = INF_TIMEOUT;
    MRT = INF_MAX_RT;
    MRC = 0;
    MRD = 0;
    RT= 0 ;

    Iface=iface->getID();
    IsDone=false;

    Options.append(new TClntOptClientIdentifier(ClntCfgMgr->getDUID(),this));
    Options.append(new TClntOptElapsed(this));

    SmartPtr<TClntOptOptionRequest> ptrOptReqOpt=new TClntOptOptionRequest(iface,this);
    Options.append((Ptr*)ptrOptReqOpt);
    if (iface->isReqDNSSrv()&&(iface->getDNSSrvState()==NOTCONFIGURED))
    {
        SmartPtr<TClntOptDNSServers> ptrDNSOpt=
            new TClntOptDNSServers(iface->getProposedDNSSrv(),this);
        Options.append((Ptr*)ptrDNSOpt);
        iface->setDNSSrvState(INPROCESS);
    }
    if (iface->isReqNTPSrv()&&(iface->getNTPSrvState()==NOTCONFIGURED))
    {
        SmartPtr<TClntOptNTPServers> ptrNTPOpt=
            new TClntOptNTPServers(iface->getProposedNTPSrv(),this);
        Options.append((Ptr*)ptrNTPOpt);
        iface->setNTPSrvState(INPROCESS);
    }

    if (iface->isReqDomainName()&&(iface->getDomainNameState()==NOTCONFIGURED))
    {
        SmartPtr<TClntOptDomainName> ptrDomainOpt=
            new TClntOptDomainName(iface->getProposedDomainName(),this);
        Options.append((Ptr*)ptrDomainOpt);
        iface->setDomainNameState(INPROCESS);
    }

    if (iface->isReqTimeZone()&&(iface->getTimeZoneState()==NOTCONFIGURED))
    {
        SmartPtr<TClntOptTimeZone> ptrTimeOpt=
            new TClntOptTimeZone(iface->getProposedTimeZone(),this);
        Options.append((Ptr*)ptrTimeOpt);
        iface->setTimeZoneState(INPROCESS);
    }
    if (!ptrOptReqOpt->getOptCnt())
        IsDone=true;
    else
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
    
    std::clog << logger::logDebug << "Create Information Request"<<logger::endl;
    //Filter options in case of options: server identifier
    
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
            case OPTION_IA:
            case OPTION_IAADDR:
            case OPTION_RAPID_COMMIT:
            case OPTION_INTERFACE_ID:
            case OPTION_RECONF_MSG:
                Options.del();
                break;        
        }
        //The other options can be included in Information request option
        //CLIENTID,ORO,ELAPSED_TIME,AUTH_MSG,USER_CLASS,VENDOR_CLASS,
        //VENDOR_OPTS,DNS_RESOLVERS,DOMAIN_LIST,NTP_SERVERS,TIME_ZONE,
        //RECONF_ACCEPT - maybe also SERVERID if information request
        //is answer to reconfigure message
    }
    pkt = new char[getSize()];
}


void TClntMsgInfRequest::answer(SmartPtr<TMsg> msg)
{
    //server DUID from which there is answer
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) msg->getOption(OPTION_SERVERID);
    //which option have we requested from server
    SmartPtr<TClntOptOptionRequest> ptrOptionReqOpt;
    ptrOptionReqOpt = (Ptr*)getOption(OPTION_ORO);
    //analyze the reveived packet and check if all options
    //were provided
    bool newOptionAssigned=false;
    
    SmartPtr<TOpt> option;
    msg->firstOption();
    while(option = msg->getOption())
    {
        option->setParent(&(*msg));
        //if option did what it was supposed to do ???
        if (option->doDuties()) 
        {
            SmartPtr<TOpt> requestOpt;
            this->firstOption();
            while ( requestOpt = getOption()) 
            {
                if (requestOpt->getOptType()==option->getOptType())
                {
                    if (ptrOptionReqOpt&&(ptrOptionReqOpt->isOption(option->getOptType())))
                        ptrOptionReqOpt->delOption(option->getOptType());
                    this->Options.del();
                    newOptionAssigned=true;
                }//if
            }//while
        }
    }

    if (ptrOptionReqOpt&&ptrOptionReqOpt->getOptCnt()&&newOptionAssigned)
    {
        clog<<logger::logNotice<<"Not all options were delivered."
            <<" Sendding new Inormation Request." << logger::endl;
        ClntTransMgr->sendInfRequest(Options,Iface);
    }
    if (newOptionAssigned) 
        IsDone=true;
    return;
}

void TClntMsgInfRequest::doDuties()
{
    // mamy timeout i nadal nie ma odpowiedzi, retransmisja
    //uplynal ostateczny timeout dla wiadomosci
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
    delete pkt;
    pkt=NULL;
}
