#include "SmartPtr.h"
#include "ClntMsgConfirm.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptStatusCode.h"
#include "ClntOptIA_NA.h"
#include "DHCPConst.h"
#include "Logger.h"

//  iaLst - contain all IA's to  be checked (they have to be in the same link)
TClntMsgConfirm::TClntMsgConfirm(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				 SmartPtr<TClntTransMgr> TransMgr, 
				 SmartPtr<TClntCfgMgr>	CfgMgr, 
				 SmartPtr<TClntAddrMgr> AddrMgr,
				 unsigned int iface, 
				 TContainer<SmartPtr<TAddrIA> > iaLst)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, SmartPtr<TIPv6Addr>() /*NULL*/,CONFIRM_MSG) {   

    IRT = CNF_TIMEOUT;
    MRT = CNF_MAX_RT;
    MRC = 0;
    MRD = CNF_MAX_RD;
 
    //The client sets the "msg-type" field to CONFIRM.  The client
    //generates a transaction ID and inserts this value in the
    //"transaction-id" field.

    //The client MUST include a Client Identifier option to identify itself
    //to the server.  
    Options.append(new TClntOptClientIdentifier( CfgMgr->getDUID(), this ) );
    //The client includes IA options for all of the IAs
    //assigned to the interface for which the Confirm message is being
    //sent.  The IA options include all of the addresses the client
    //currently has associated with those IAs.  The client SHOULD set the
    //T1 and T2 fields in any IA_NA options, and the preferred-lifetime and
    //valid-lifetime fields in the IA Address options to 0, as the server
    //will ignore these fields. 
    SmartPtr<TAddrIA> ia;
    iaLst.first();
    while(ia=iaLst.get())
        Options.append(new TClntOptIA_NA(ia,true,this));
    pkt = new char[getSize()];
}           

void TClntMsgConfirm::answer(SmartPtr<TMsg> reply)
{
    SmartPtr <TClntOptStatusCode> ptrOptCode;
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
    case  STATUSCODE_UNSPECFAIL:
    case STATUSCODE_NOADDRSAVAIL:
    case STATUSCODE_NOBINDING:
    case STATUSCODE_USEMULTICAST:
    default:
	std::clog << logger::logWarning << "REPLY for CONFIRM received with invalid (" 
		  << ptrOptCode->getCode() << ") status code." << logger::endl;
	break;
    }
    return;

}

void TClntMsgConfirm::addrsAccepted() {
    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TOpt> ptrOpt;
    this->firstOption();
    while ( ptrOpt = this->getOption() ) {
	if (ptrOpt->getOptType()!=OPTION_IA)
	    continue;

	SmartPtr<TClntOptIA_NA> ptrOptIA = (Ptr*) ptrOpt;
	ptrIA = ClntAddrMgr->getIA( ptrOptIA->getIAID() );
	if (!ptrIA)
	    continue;

	// set them to RENEW timeout
	unsigned long ts =now()-ptrIA->getT1();
	ptrIA->setTimestamp(ts);
	ptrIA->setState(CONFIGURED);
    }
    ClntAddrMgr->firstIA();
}

void TClntMsgConfirm::addrsRejected() {
    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TOpt> ptrOpt;
    this->firstOption();
    while ( ptrOpt = this->getOption() ) {
	if (ptrOpt->getOptType()!=OPTION_IA)
	    continue;

	SmartPtr<TClntOptIA_NA> ptrOptIA = (Ptr*) ptrOpt;
	ptrIA = ClntAddrMgr->getIA(ptrOptIA->getIAID());
	if (!ptrIA)
	    continue;

	// release all addrs
	SmartPtr<TIfaceIface> ptrIface;
	ptrIface = ClntIfaceMgr->getIfaceByID(ptrIA->getIface());
	if (!ptrIface) {
	    std::clog << logger::logCrit << "We have addresses assigned to non-existing interface."
		"Help! Somebody stole interface!" << logger::endl;
	    ptrIA->setState(NOTCONFIGURED);
	    return;
	}
	SmartPtr<TAddrAddr> ptrAddr;
	ptrIA->firstAddr();
	while (ptrAddr = ptrIA->getAddr() ) {
	    // remove addr from ...
	    ptrIface->delAddr( ptrAddr->get() );
	    // ... and from DB
	    ptrIA->delAddr( ptrAddr->get() );
	}
	ptrIA->setState(NOTCONFIGURED);
    }
    ClntAddrMgr->firstIA();
}

void TClntMsgConfirm::doDuties()
{	
    
    //The first Confirm message from the client on the interface MUST be
    //delayed by a random amount of time between 0 and CNF_MAX_DELAY.    
    // FIXME:
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

TClntMsgConfirm::~TClntMsgConfirm()
{
    delete pkt;
}
