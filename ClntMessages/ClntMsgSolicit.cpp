/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * chamges: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgSolicit.cpp,v 1.18 2006-11-03 23:14:40 thomson Exp $
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
#include "ClntOptServerIdentifier.h"
#include <cmath>
#include "Logger.h"

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
    MRC=0; //these both below mean there is no ending condition and transactions
    MRD=0; //lasts till receiving answer
    RT=0;
	
    // ClientIdentifier option
    SmartPtr<TOpt> ptr;
    ptr = new TClntOptClientIdentifier( CfgMgr->getDUID(), this );
    Options.append( ptr );
    
    // all IAs provided by checkSolicit
    SmartPtr<TClntCfgIA> ClntCfgIA;
    IAs.first();
    while (ClntCfgIA = IAs.get()) {
	SmartPtr<TClntOptIA_NA> IA_NA;
	IA_NA = new TClntOptIA_NA(ClntCfgIA, this);
	Options.append((Ptr*)IA_NA);
    }
    
    // include ELAPSED option
    Options.append(new TClntOptElapsed(this));

    if(rapid)
        Options.append(new TClntOptRapidCommit(this));

    this->appendTAOptions(true); // append and switch to INPROCESS state

    // append options specified in the config file
    this->appendRequestedOptions();
    
    pkt = new char[getSize()];
}

void TClntMsgSolicit::answer(SmartPtr<TClntMsg> msg)
{
    if (shallRejectAnswer(msg))
	return;

    switch (msg->getType()) {
    case ADVERTISE_MSG:
    {
	if (this->getOption(OPTION_RAPID_COMMIT)) {
	    Log(Info) << "Server responded with ADVERTISE instead of REPLY, probably does not support"
		" RAPID-COMMIT." << LogEnd;
	}
	AnswersLst.append((Ptr*)msg);
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
	if (!this->getOption(OPTION_RAPID_COMMIT)) {
	    Log(Warning) << "REPLY received, but SOLICIT was sent without RAPID_COMMIT. Ingoring." 
			 << LogEnd;
	    return;
	}
	if (!msg->getOption(OPTION_RAPID_COMMIT)) {
	    Log(Warning) << "REPLY as answer for SOLICIT received without RAPID_COMMIT. Ignoring."
			 << LogEnd;
	    return;
	}

	this->replyReceived(msg);
	IsDone=true;
	break;
    }
    default:
	Log(Warning) << "Invalid message type (" << msg->getType() 
		     << ") received as answer for SOLICIT message." << LogEnd;
	return;
    }
 }

/*
 * use REPLY provided in rapid-commit situation
 */
void TClntMsgSolicit::replyReceived(SmartPtr<TClntMsg> msg) {
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) msg->getOption(OPTION_SERVERID);

    if (!ptrDUID) {
	Log(Warning) << "REPLY (for SOLICIT with rapid-commit) recevied without SERVER-ID option." 
		     << LogEnd;
	this->IsDone = true;
	return;
    }
    
    // analyse all options received
    SmartPtr<TOpt> option;

    // find ORO in received options
    msg->firstOption();
    SmartPtr<TClntOptOptionRequest> ptrOptionReqOpt = (Ptr*) msg->getOption(OPTION_ORO);

    msg->firstOption();
    while (option = msg->getOption() ) 
    {
        switch (option->getOptType()) 
        {
            case OPTION_IA:
            {
                SmartPtr<TClntOptIA_NA> clntOpt = (Ptr*)option;
                clntOpt->setThats(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
				  ptrDUID->getDUID(), SmartPtr<TIPv6Addr>()/*NULL*/, this->Iface);

                clntOpt->doDuties();

                if (clntOpt->getStatusCode()==STATUSCODE_SUCCESS)
                {
                    // if we have received enough addresses,
		    // remove assigned IA's from request message
                    SmartPtr<TOpt> requestOpt;
                    this->Options.first();
                    while (requestOpt = this->Options.get())
                    {
                        if (requestOpt->getOptType()==OPTION_IA)
                        {
                            SmartPtr<TClntOptIA_NA> ptrIA = (Ptr*) requestOpt;
                            if ((ptrIA->getIAID() == clntOpt->getIAID() ) &&
                                (ClntCfgMgr->countAddrForIA(ptrIA->getIAID()) == ptrIA->countAddr()) )	
                            {
                                //found this IA, it has enough addresses and everything is ok.
                                //Shortly, we have this IA configured.
                                this->Options.del();
                                break;
                            }
                        } //if
                    } //while
                }
                break;
            }
            case OPTION_IAADDR:
                Log(Warning) << "Option OPTION_IAADDR misplaced." << LogEnd;
                break;
            default:
            {
                option->setParent(this);
                if (option->doDuties()) 
                {
                    SmartPtr<TOpt> requestOpt;
                    this->Options.first();
                    while ( requestOpt = this->Options.get()) {
                        if ( requestOpt->getOptType() == option->getOptType() ) 
                        {
                            if (ptrOptionReqOpt&&(ptrOptionReqOpt->isOption(option->getOptType())))
                                ptrOptionReqOpt->delOption(option->getOptType());
                            this->Options.del();
                        }//if
                    }//while
                }
            }
        }
    }
    //Options and IAs serviced by server are removed from requestOptions list

    SmartPtr<TOpt> solicitOpt;
    this->Options.first();
    bool IAsToConfigure = false;
    while ( solicitOpt = this->Options.get()) {
        if (solicitOpt->getOptType() == OPTION_IA) {
	    SmartPtr<TClntOptIA_NA> optIA = (Ptr*) solicitOpt;
	    SmartPtr<TAddrIA> addrIA = ClntAddrMgr->getIA(optIA->getIAID());
	    addrIA->setState(NOTCONFIGURED);
	    Log(Info) << "IA (IAID=" << addrIA->getIAID() << ") still not configured." << LogEnd;
	    IAsToConfigure = true;
            break;
        }
    }

    // are there any options (requested in REQUEST_OPTION) not yet configured?
    if ( ptrOptionReqOpt && (ptrOptionReqOpt->count()) )
    {
	Log(Notice) << "All IA(s) were supplied, but not all requested options."
	  << "Sending Information Request" << LogEnd;
	ClntTransMgr->sendInfRequest(this->Options, this->Iface);
    }

    IsDone = true;
    return;
}

/** 
 * check if received message should be accepted. Following conditions are checked:
 * - is server on the black-list?
 * - are all requested options present?
 * - is there requested IA option?
 * - is there requested TA option?
 * 
 * @param msg 
 * 
 * @return 
 */
bool TClntMsgSolicit::shallRejectAnswer(SmartPtr<TClntMsg> msg)
{
    // this == solicit or request
    // msg  == reply
    SmartPtr<TClntOptServerIdentifier> srvDUID = (Ptr*) msg->getOption(OPTION_SERVERID);
    if (!srvDUID) {
	Log(Notice) << "No server identifier provided. Message ignored." << LogEnd;
	return true;
    }
    
    //is this server rejected?
    SmartPtr<TClntCfgIface> iface = ClntCfgMgr->getIface(this->Iface);
    if (!iface) {
	Log(Error) << "Unable to find iface=" << this->Iface << "." << LogEnd;
	return false;
    }

    if (iface->isServerRejected(msg->getAddr(), srvDUID->getDUID())) {
	Log(Notice) << "Server was rejected (duid=" << srvDUID->getDUID() << ")." << LogEnd;
        return true;
    }

    // have we asked for IA?
    if ( (this->getOption(OPTION_IA)) && (!msg->getOption(OPTION_IA)) ) {
	Log(Notice) << "IA option requested, but not present in this message. Ignored." << LogEnd;
	return true;
    }
    
    // have we asked for TA?
    if ( (this->getOption(OPTION_IA_TA)) && (!msg->getOption(OPTION_IA_TA)) ) {
	Log(Notice) << "TA option requested, but not present in this message. Ignored." << LogEnd;
    }
	 
    // everything seems ok
    return false;
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
