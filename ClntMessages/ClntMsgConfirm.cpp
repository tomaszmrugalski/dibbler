/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgConfirm.cpp,v 1.14 2009-03-24 23:17:17 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "ClntMsgConfirm.h"
#include "OptDUID.h"
#include "ClntOptStatusCode.h"
#include "ClntOptIA_NA.h"
#include "DHCPConst.h"
#include "Logger.h"

//  iaLst - contain all IA's to  be checked (they have to be in the same link)
TClntMsgConfirm::TClntMsgConfirm(unsigned int iface, 
                                 List(TAddrIA) iaLst)
  :TClntMsg(iface, 0, CONFIRM_MSG) {   
  
    IRT = CNF_TIMEOUT;
    MRT = CNF_MAX_RT;
    MRC = 0;
    MRD = CNF_MAX_RD;
 
    //The client sets the "msg-type" field to CONFIRM.  The client
    //generates a transaction ID and inserts this value in the
    //"transaction-id" field.

    //The client MUST include a Client Identifier option to identify itself
    //to the server.  
    Options.push_back(new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(), this ) );
    //The client includes IA options for all of the IAs
    //assigned to the interface for which the Confirm message is being
    //sent.  The IA options include all of the addresses the client
    //currently has associated with those IAs.  The client SHOULD set the
    //T1 and T2 fields in any IA_NA options, and the preferred-lifetime and
    //valid-lifetime fields in the IA Address options to 0, as the server
    //will ignore these fields. 
    SPtr<TAddrIA> ia;
    iaLst.first();
    while(ia=iaLst.get())
        Options.push_back(new TClntOptIA_NA(ia,true,this));

    appendRequestedOptions();
    appendElapsedOption();
    appendAuthenticationOption();
    
    IsDone = false;
    send();
}           
    
void TClntMsgConfirm::answer(SPtr<TClntMsg> reply)
{
    SPtr <TClntOptStatusCode> ptrOptCode;
    ptrOptCode = (Ptr*) reply->getOption(OPTION_STATUS_CODE);
    switch (ptrOptCode->getCode() ) {
    case STATUSCODE_SUCCESS:
	this->addrsAccepted();
	this->IsDone = true;
	break;
    case STATUSCODE_NOTONLINK:
	this->addrsRejected();
	this->IsDone = true;
	break;
    case STATUSCODE_UNSPECFAIL:
    case STATUSCODE_NOADDRSAVAIL:
    case STATUSCODE_NOBINDING:
    case STATUSCODE_USEMULTICAST:
    default:
	Log(Warning) << "REPLY for CONFIRM received with invalid (" 
		     << ptrOptCode->getCode() << ") status code." << LogEnd;
	break;
    }
    return;

}

void TClntMsgConfirm::addrsAccepted() {
    SPtr<TAddrIA> ptrIA;
    SPtr<TOpt> ptrOpt;
    this->firstOption();
    SPtr<TAddrAddr> ptrAddrAddr;
    while ( ptrOpt = this->getOption() ) {
	if (ptrOpt->getOptType()!=OPTION_IA_NA)
	    continue;

	SPtr<TClntOptIA_NA> ptrOptIA = (Ptr*) ptrOpt;
	ptrIA = ClntAddrMgr().getIA( ptrOptIA->getIAID() );
	if (!ptrIA)
	    continue;

	//CHANGED: ??why ts=now()-ptrIA->getT1(); So comment this line.	
	// set them to RENEW timeout
	// Uncomment this line if you don't want RENEW to be sent after
	// CONFIRM exchange is complete
	// ptrIA->setTimestamp( now()-ptrIA->getT1() );
	ptrIA->setState(STATE_CONFIGURED);
    }
    ClntAddrMgr().firstIA();
	SPtr<TIfaceIface> ptrIface;
	ptrIface = ClntIfaceMgr().getIfaceByID(ptrIA->getIface());
	ptrIA->firstAddr();
	ptrAddrAddr = ptrIA->getAddr();
	ptrIface->addAddr(ptrAddrAddr->get(),ptrAddrAddr->getPref(),ptrAddrAddr->getValid(),ptrIface->getPrefixLength());
}

void TClntMsgConfirm::addrsRejected() {
    SPtr<TAddrIA> ptrIA;
    SPtr<TOpt> ptrOpt;
    this->firstOption();
    while ( ptrOpt = this->getOption() ) {
	if (ptrOpt->getOptType()!=OPTION_IA_NA)
	    continue;

	SPtr<TClntOptIA_NA> ptrOptIA = (Ptr*) ptrOpt;
	ptrIA = ClntAddrMgr().getIA(ptrOptIA->getIAID());
	if (!ptrIA)
	    continue;

	// release all addrs
	SPtr<TIfaceIface> ptrIface;
	ptrIface = ClntIfaceMgr().getIfaceByID(ptrIA->getIface());
	if (!ptrIface) {
	    Log(Crit) << "We have addresses assigned to non-existing interface."
		"Help! Somebody stole an interface!" << LogEnd;
	    ptrIA->setState(STATE_NOTCONFIGURED);
	    return;
	}
	SPtr<TAddrAddr> ptrAddr;
	ptrIA->firstAddr();
	while (ptrAddr = ptrIA->getAddr() ) {
	    // remove addr from ...
	    ptrIface->delAddr( ptrAddr->get(), ptrIface->getPrefixLength() );
	    // ... and from DB
	    ptrIA->delAddr( ptrAddr->get() );
	}

	//not only the address should be rejected, but also the original IA should be deleted and using a new IA to process SOLICIT
	SPtr<TClntCfgIA> ptrCfgIA = ClntCfgMgr().getIA(ptrOptIA->getIAID());
	ptrCfgIA->reset();
	ptrIA->reset();
    }
    ClntAddrMgr().firstIA();
}

void TClntMsgConfirm::doDuties()
{	
    
    //The first Confirm message from the client on the interface MUST be
    //delayed by a random amount of time between 0 and CNF_MAX_DELAY.    
    /// @todo:
        //if (RC == 0) ;
		//microsleep(rand()%1000000);
    if (!MRD) {
	// MRD reached. Nobody said that out addrs are faulty, so we suppose
	// they are ok. Use them
	this->addrsAccepted();
	this->IsDone = true;
	return;
    }
	
    send();
}

unsigned long TClntMsgConfirm::getTimeout()
{
	return 0;
}

bool TClntMsgConfirm::check()
{
	return 0;
}

string TClntMsgConfirm::getName() {
    return "CONFIRM";
}

TClntMsgConfirm::~TClntMsgConfirm() {
}
