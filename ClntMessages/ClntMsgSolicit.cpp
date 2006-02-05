/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgSolicit.cpp,v 1.13.2.1 2006-02-05 23:38:07 thomson Exp $
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

    this->appendTAOptions(false); // append, but don't switch to INPROCESS state

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

bool TClntMsgSolicit::shallRejectAnswer(SmartPtr<TClntMsg> msg)
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

/*
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2005/01/08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.12  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.11  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.10  2004/10/02 13:11:24  thomson
 * Boolean options in config file now can be specified with YES/NO/TRUE/FALSE.
 * Unicast communication now can be enable on client side (disabled by default).
 *
 * Revision 1.9  2004/09/07 22:02:32  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 * Revision 1.8  2004/09/03 23:20:22  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 * Revision 1.6  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.5  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */
