/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * chamges: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgSolicit.cpp,v 1.26 2008-08-29 00:07:28 thomson Exp $
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
#include "ClntOptTA.h"
#include "ClntOptIA_PD.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptElapsed.h"
#include "ClntOptPreference.h"
#include "ClntOptRapidCommit.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptStatusCode.h"
#include <cmath>
#include "Logger.h"

TClntMsgSolicit::TClntMsgSolicit(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr>   CfgMgr,
				 SmartPtr<TClntAddrMgr>  AddrMgr,
				 int iface, SmartPtr<TIPv6Addr> addr,
				 List(TClntCfgIA) iaLst, 
				 SPtr<TClntCfgTA> ta,
				 List(TClntCfgPD) pdLst, 
				 bool rapid)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,SOLICIT_MSG)
{
    IRT=SOL_TIMEOUT;
    MRT=SOL_MAX_RT;
    MRC=0; //these both below mean there is no ending condition and transactions
    MRD=0; //lasts till receiving answer
    RT=0;
	
    // ClientIdentifier option
    appendClientID();
    
    // all IAs are provided by ClntTransMgr::checkSolicit()
    SmartPtr<TClntCfgIA> ia;
    iaLst.first();
    while (ia = iaLst.get()) {
	SmartPtr<TClntOptIA_NA> iaOpt;
	iaOpt = new TClntOptIA_NA(ia, this);
	Options.append( (Ptr*)iaOpt );
	ia->setState(STATE_INPROCESS);
    }

    // TA is provided by ClntTransMgr::checkSolicit()
    if (ta) {
	SPtr<TClntOptTA> taOpt = new TClntOptTA(ta->getIAID(), this);
	Options.append( (Ptr*) taOpt);
	ta->setState(STATE_INPROCESS);
    }

    // all PDs are provided by ClntTransMgr::checkSolicit()
    SPtr<TClntCfgPD> pd;
    pdLst.first();
    while ( pd = pdLst.get() ) {
	SPtr<TClntOptIA_PD> pdOpt = new TClntOptIA_PD(pd, this);
	Options.append( (Ptr*)pdOpt );
	pd->setState(STATE_INPROCESS);
    }
    
    if(rapid)
        Options.append(new TClntOptRapidCommit(this));

    // append and switch to INPROCESS state
    this->appendTAOptions(true); 

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
	ClntTransMgr->addAdvertise((Ptr*)msg);
	SmartPtr<TOptPreference> prefOpt = (Ptr*) msg->getOption(OPTION_PREFERENCE);
	if (prefOpt && (prefOpt->getPreference() == 255) )
	{
	    Log(Info) << "ADVERTISE message with prefrence set to 255 received, so wait time for"
		" other possible ADVERTISE messages is skipped." << LogEnd;
	    ClntTransMgr->sendRequest(Options,Iface);
	    IsDone = true;
	    return;
	}
	if (this->RC > 1)
	{
	    ClntTransMgr->sendRequest(Options,Iface);
	    IsDone = true;
	    return;
	}
	break;
    }
    case REPLY_MSG:
    {
	if (!this->getOption(OPTION_RAPID_COMMIT)) {
	    Log(Warning) << "REPLY received, but SOLICIT was sent without RAPID_COMMIT. Ignoring." 
			 << LogEnd;
	    return;
	}
	if (!msg->getOption(OPTION_RAPID_COMMIT)) {
	    Log(Warning) << "REPLY as answer for SOLICIT received without RAPID_COMMIT. Ignoring."
			 << LogEnd;
	    return;
	}

	TClntMsg::answer(msg);
	break;
    }
    default:
	Log(Warning) << "Invalid message type (" << msg->getType() 
		     << ") received as answer for SOLICIT message." << LogEnd;
	return;
    }
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
    if (this->getOption(OPTION_IA_NA))
    {
	SPtr<TClntOptIA_NA> ia = (Ptr*)msg->getOption(OPTION_IA_NA);
	if (!ia)  {
	    Log(Notice) << "IA_NA option requested, but not present in this message. Ignored." << LogEnd;
	    return true;
	}
	if (!ia->getOption(OPTION_IAADDR)) {
	    Log(Notice) << "IA_NA option returned, but without any addresses. Ignored." << LogEnd;
	    return true;
	}
	SPtr<TClntOptStatusCode> st = (Ptr*)ia->getOption(OPTION_STATUS_CODE);
	if (st && st->getCode()!= STATUSCODE_SUCCESS) {
	    Log(Notice) << "IA_NA has status code!=SUCCESS: " << st->getCode() << "(" << st->getText() << "). Ignored." << LogEnd;
	    return true;
	}
    }
    

    
    // have we asked for TA?
    if ( (this->getOption(OPTION_IA_TA)) && (!msg->getOption(OPTION_IA_TA)) ) {
	Log(Notice) << "TA option requested, but not present in this message. Ignored." << LogEnd;
	return true;
    }

    // have we asked for PD?
    if ( (this->getOption(OPTION_IA_PD)) && (!msg->getOption(OPTION_IA_PD)) ) {
	Log(Notice) << "TA option requested, but not present in this message. Ignored." << LogEnd;
	return true;
    }
	 
    // everything seems ok
    return false;
}

void TClntMsgSolicit::doDuties()
{
    if ( ClntTransMgr->getAdvertiseLstCount() ) 
    { // there is a timeout, but we have already answers and all is ok
	ClntTransMgr->sendRequest(Options, Iface);
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
