/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgSolicit.cpp,v 1.7 2004-08-24 22:48:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.5  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 * Revision 1.4  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 * Revision 1.3  2004/04/12 21:46:59  thomson
 * *** empty log message ***
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include "SmartPtr.h"
#include "Msg.h"

#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "ClntCfgIA.h"

#include "ClntMsg.h"
#include "ClntMsgSolicit.h"
#include "ClntMsgAdvertise.h"

#include "ClntOptIA_NA.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptElapsed.h"
#include "ClntOptPreference.h"
#include "ClntOptRapidCommit.h"
#include "ClntOptDNSServers.h"
#include "ClntOptNTPServers.h"
#include "ClntOptDomainName.h"
#include "ClntOptTimeZone.h"
#include "ClntOptServerIdentifier.h"
#include <cmath>

TClntMsgSolicit::TClntMsgSolicit(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr>   CfgMgr,
				 SmartPtr<TClntAddrMgr>  AddrMgr,
				 int iface, SmartPtr<TIPv6Addr> addr, 
                 List(TClntCfgIA) IAs, bool rapid)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,SOLICIT_MSG)
{
	IRT=SOL_TIMEOUT;
	MRT=SOL_MAX_RT;
	//these both below mean there is no ending condition and transactions
	//lasts till receiving answer
	MRC=0;
	MRD=0;
	
	RT=0;
	
	// ClientIdentifier option
	SmartPtr<TOpt> ptr;
	ptr = new TClntOptClientIdentifier( CfgMgr->getDUID(), this );
	Options.append( ptr );

	// all IAs provided by checkSolicit
	SmartPtr<TClntCfgIA> ClntCfgIA;
	IAs.first();
	while (ClntCfgIA = IAs.get()) 
	{
	    SmartPtr<TClntOptIA_NA> IA_NA;
	    IA_NA = new TClntOptIA_NA(ClntCfgIA, this);
	    Options.append((Ptr*)IA_NA);
	}
	// what should we ask for (besides addrs)?
	//SmartPtr<TOpt> ptrOpt;
	//ptrOpt = new TClntOptOptionRequest(CfgMgr);
	//Options.append( ptrOpt );
    
	// include ELAPSED option
	Options.append(new TClntOptElapsed(this));
    //Rapid Commit
    IAs.first();
    ClntCfgIA=IAs.get();
    //FIXME:whether send solicit with rapid commit option shlould
    //      be passed by additional parameter in constructor
    //      why - look at readme.txt file
    if(rapid)
        Options.append(new TClntOptRapidCommit(this));
    
    SmartPtr<TClntOptOptionRequest> ptrOptOptReq=
        new TClntOptOptionRequest(CfgMgr->getIface(Iface),this);
    Options.append((Ptr*)ptrOptOptReq);

    SmartPtr<TClntCfgIface> ptrIface;
    if (ptrIface=CfgMgr->getIface(iface))
    {
        if (ptrIface->isReqDNSSrv()&&(ptrIface->getDNSSrvState()==NOTCONFIGURED))
        {
            SmartPtr<TClntOptDNSServers> ptrDNSOpt=
                new TClntOptDNSServers(ptrIface->getProposedDNSSrv(),this);
            Options.append((Ptr*)ptrDNSOpt);
        }

        if (ptrIface->isReqNTPSrv()&&(ptrIface->getNTPSrvState()==NOTCONFIGURED))
        {
            SmartPtr<TClntOptNTPServers> ptrNTPOpt=
                new TClntOptNTPServers(ptrIface->getProposedNTPSrv(),this);
            Options.append((Ptr*)ptrNTPOpt);
        }
        
        if (ptrIface->isReqDomainName()&&(ptrIface->getDomainNameState()==NOTCONFIGURED))
        {
            SmartPtr<TClntOptDomainName> ptrDomainOpt=
                new TClntOptDomainName(ptrIface->getProposedDomainName(),this);
            Options.append((Ptr*)ptrDomainOpt);
        }

        if (ptrIface->isReqTimeZone()&&(ptrIface->getTimeZoneState()==NOTCONFIGURED))
        {
            SmartPtr<TClntOptTimeZone> ptrTimeOpt=
                new TClntOptTimeZone(ptrIface->getProposedTimeZone(),this);
            Options.append((Ptr*)ptrTimeOpt);
        }
    }

    //FIXME: If there will be enough time, implement options:
    //UserClass i ReconfigureAccept should appear in this 
    //part of code
    
    pkt = new char[getSize()];
}

void TClntMsgSolicit::answer(SmartPtr<TMsg> msg)
{
    if (shallRejectAnswer(msg))
	return;

    switch (msg->getType()) {
    case ADVERTISE_MSG:
    {
	AnswersLst.append(msg);
	SmartPtr<TOptPreference> prefOpt = (Ptr*) msg->getOption(OPTION_PREFERENCE);
	if (prefOpt && (prefOpt->getPreference() == 255) )
	{
	    Log(Info) << "ADVERTISE message with prefrence set to 255 received, so wait time for"
		" other possible ADVERTISE messages is skipped." << LogEnd;
	    sortAnswers();
	    ClntTransMgr->sendRequest(Options,AnswersLst,Iface);
	    IsDone = true;
	    return;
	}
	if (this->RC > 1)
	{
	    sortAnswers();
	    ClntTransMgr->sendRequest(Options,AnswersLst,Iface);
	    IsDone = true;
	    return;
	}
	break;
    }
    case REPLY_MSG:
    {
	if (!this->getOption(OPTION_RAPID_COMMIT))
	{
	    Log(Warning) << "REPLY received, but SOLICIT was sent without RAPID_COMMIT. Ingoring." 
			 << LogEnd;
	    return;
	}
	if (!msg->getOption(OPTION_RAPID_COMMIT))
	{
	    Log(Warning) << "REPLY as answer for SOLICIT received without RAPID_COMMIT. Ignoring."
			 << LogEnd;
	    return;
	}

	sortAnswers();
	IsDone=true;
	break;
    }
    default:
	Log(Warning) << "Invalid message type (" << msg->getType() 
		     << ") received as answer for SOLICIT message." << LogEnd;
	return;
    }
 }

//Check reveived message against following conditions:
//  + is received from appropriate server (not rejected)
//  + contains all require options(see cfg. manager and prefix require) 
//  i.e..:
//      - DNS Server Option
//      - NTP Server Option
//      - Domain Search List Option
//      - Time Zone Option
//
//  + at least one IA has appropriate number of addresses

bool TClntMsgSolicit::shallRejectAnswer(SmartPtr<TMsg> msg)
{
    //get option IA_NA it must be included-it's a solicit message
    SmartPtr<TClntOptIA_NA> ptrSolIA = (Ptr*) this->getOption(OPTION_IA);
    //find grooup of ia's - it contains information of rejected servers
    SmartPtr<TClntCfgGroup> ptrCfgGroup = ClntCfgMgr->getGroupForIA(ptrSolIA->getIAID());
    SmartPtr<TClntOptServerIdentifier> ptrSrvDUID = 
                                    (Ptr*) msg->getOption(OPTION_SERVERID);
    //is this server rejected
    if (ptrCfgGroup->isServerRejected(msg->getAddr(),ptrSrvDUID->getDUID()))
        return true;

    SmartPtr<TClntOptOptionRequest> ptrReqOpt = (Ptr*) getOption(OPTION_ORO);
    SmartPtr<TOpt> ptrAnswOpt;
    SmartPtr<TClntCfgIface> ptrIface=ClntCfgMgr->getIface(this->Iface);
    //Check required options
    if (ptrReqOpt&&(ptrReqOpt->countOption()))
    {
        for(int i=0;i<ptrReqOpt->countOption();i++)
        {
            switch(ptrReqOpt->getReqOpt(i))
            {
            case OPTION_DNS_RESOLVERS:
                if (ptrIface->getDNSReqOpt()==Require)
                    if (!(msg->getOption(OPTION_DNS_RESOLVERS)))
                        return true;
                break;
            case OPTION_DOMAIN_LIST:
                if (ptrIface->getDomainReqOpt()==Require)
                    if (!(msg->getOption(OPTION_DOMAIN_LIST)))
                        return true;
                break;
            case OPTION_NTP_SERVERS:
                if (ptrIface->getDomainReqOpt()==Require)
                    if (!(msg->getOption(OPTION_DOMAIN_LIST)))
                        return true;
            case OPTION_TIME_ZONE:
                if (ptrIface->getDomainReqOpt()==Require)
                    if (!(msg->getOption(OPTION_DOMAIN_LIST)))
                        return true;
                break;
            }
        };
    }
    //check at last for ia inclusion
    msg->firstOption();
    while(ptrAnswOpt=msg->getOption())
    {
        if (ptrAnswOpt->getOptType()==OPTION_IA)
        {
            SmartPtr<TClntOptIA_NA> ptrAnswIA=(Ptr*)ptrAnswOpt;
            SmartPtr<TClntCfgIA> ptrQuestIA = ClntCfgMgr->getIA(ptrAnswIA->getIAID());
            if (ptrQuestIA->countAddr()<=ptrAnswIA->countAddr())
                return false;    //there is at least one IA
        }
    }
    //No there is no such a IA
    return true;
}

int TClntMsgSolicit::getMaxPreference()
{
    if (AnswersLst.count() == 0)
	return -1;
    int max = 0;
    
    SmartPtr<TClntMsgAdvertise> ptr;
    AnswersLst.first();
    while ( ptr = (Ptr*) AnswersLst.get() ) {
	if ( max < ptr->getPreference() )
	    max = ptr->getPreference();
    }
    return max;
}

void TClntMsgSolicit::sortAnswers()
{
    // delete not wanted servers from AnswersLst
    // FIXME: (get info from cfgMgr)

    // we'll store all ADVERTISE here 
    List(TMsg) sorted;

    // sort ADVERTISE by the PREFERENCE value
    SmartPtr<TClntMsgAdvertise> ptr;
    while (AnswersLst.count()) {
	int max = getMaxPreference();
	AnswersLst.first();
	while ( ptr = (Ptr*) AnswersLst.get() ) {
	    if (ptr->getPreference() == max) 
		break;
	}
	
	// did we find it? Then append it on the end of sorted list, and delete from this new.
	if (ptr) {
	    sorted.append( (Ptr*) ptr );
	    AnswersLst.del();
	}
    }

    // now copy sorted list to AnswersLst
    AnswersLst = sorted;
}

void TClntMsgSolicit::doDuties()
{
	if (AnswersLst.count()) 
	{ // there is a timeout, but we have already answers and all is ok
		sortAnswers();
		ClntTransMgr->sendRequest(Options,AnswersLst,Iface);
		IsDone = true;
		return;
	}
	// there is a timeout and there is no still answer
	//is it a final timeout for this message

	// first transmission
	//if (RC == 0) ;
		//microsleep(rand()%1000000);
	send();
}


bool TClntMsgSolicit::check()
{
    return false;
}

string TClntMsgSolicit::getName() {
    return "SOLICIT";
}

TClntMsgSolicit::~TClntMsgSolicit()
{
    delete [] pkt;
}
