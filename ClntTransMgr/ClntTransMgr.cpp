/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntTransMgr.cpp,v 1.39 2006-03-21 20:02:02 thomson Exp $
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include <iostream>
#include <string>

#include "SmartPtr.h"
#include "ClntTransMgr.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "Msg.h"
#include "ClntMsgRequest.h"
#include "ClntMsgRenew.h"
#include "ClntMsgRebind.h"
#include "ClntMsgRelease.h"
#include "ClntMsgSolicit.h"
#include "ClntMsgInfRequest.h"
#include "ClntMsgDecline.h"
#include "ClntMsgConfirm.h"
#include "Container.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "ClntMsgDecline.h"

using namespace std;


TClntTransMgr::TClntTransMgr(SmartPtr<TClntIfaceMgr> ifaceMgr, 
			     SmartPtr<TClntAddrMgr> addrMgr,
			     SmartPtr<TClntCfgMgr> cfgMgr,
                             string config)
{
    // should we set REUSE option during binding sockets?
    this->BindReuse = CLIENT_BIND_REUSE;
    this->IsDone = true;

    this->IfaceMgr = ifaceMgr;
    this->AddrMgr  = addrMgr;
    this->CfgMgr   = cfgMgr;

    if (this->BindReuse)
	Log(Info) << "Bind reuse enabled." << LogEnd;
    else
	Log(Info) << "Bind reuse disabled." << LogEnd;

    if (!this->openLoopbackSocket()) {
	return;
    }

    SmartPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while(iface=CfgMgr->getIface()) {
	if (!this->openSocket(iface)) {
	    return;
	}
    }

    // FIXME: CONFIRM support is not complete
    // this->checkDB();
    // checkConfirm();
    this->ConfirmEnabled=true;

    this->Shutdown = false;
    this->IsDone = false;
}

bool TClntTransMgr::openSocket(SmartPtr<TClntCfgIface> iface) {

    if (iface->noConfig())       
	return true;

    SmartPtr<TIfaceIface> loopback;
    SmartPtr<TIfaceIface> ptrIface;
    IfaceMgr->firstIface();
    while (ptrIface=IfaceMgr->getIface()) {
        if (!ptrIface->flagLoopback()) {
            continue;
	}
	loopback = ptrIface;
	break;
    }
    if (!loopback) {
	Log(Error) << "Loopback interface not found!" << LogEnd;
	return false;
    }

    // create IAs in AddrMgr corresponding to those specified in CfgMgr.
    SmartPtr<TClntCfgIA> ia;
    iface->firstIA();
    while(ia=iface->getIA()) {
	AddrMgr->addIA(new TAddrIA(iface->getID(),SmartPtr<TIPv6Addr>(), 
				   SmartPtr<TDUID>(),CLIENT_DEFAULT_T1,CLIENT_DEFAULT_T2,
				   ia->getIAID()));
    }

    // open socket
    SmartPtr<TIfaceIface> realIface = IfaceMgr->getIfaceByID(iface->getID());
    if (!realIface) {
	Log(Error) << "Interface " << iface->getName() << "/" << iface->getID()
		   << " not present in system." << LogEnd;
	return false;
    }
    if (!realIface->flagUp()) {
	Log(Error) << "Interface " << realIface->getName() << "/" << realIface->getID()
		   << " is down. Unable to open socket." << LogEnd;
	return false;
    }
    if (!realIface->flagRunning()) {
	Log(Error) << "Interface " << realIface->getName() << "/" << realIface->getID()
		   << " is not running." << LogEnd;
	return false;
    }
    
    // get link-local address
    char* llAddr;
    realIface->firstLLAddress();
    llAddr=realIface->getLLAddress();
    if (!llAddr) {
	Log(Error) << "Interface " << realIface->getName() << "/" << realIface->getID()
		   << " does not have link-layer address. Weird." << LogEnd;
	return false;
    }

    SmartPtr<TIPv6Addr> addr = new TIPv6Addr(llAddr);

#if 0
#ifndef WIN32
    // required to be able to receive data from server on the same machine
    // (data is sent via the lo interface)
    Log(Notice) << "Creating socket (addr=" << *addr << ") on the " << loopback->getName() 
		<< "/" << loopback->getID() << " interface." << LogEnd;
    if (!loopback->addSocket(addr,DHCPCLIENT_PORT,false, true)) {
	Log(Crit) << "Socket creation (addr=" << *addr << ") on the " << loopback->getName() 
		  << "/" << loopback->getID() << " interface failed." << LogEnd;
	return false;
    }
#endif
#endif

    Log(Notice) << "Creating socket (addr=" << *addr << ") on the " << iface->getName() 
		<< "/" << iface->getID() << " interface." << LogEnd;
    if (!realIface->addSocket(addr,DHCPCLIENT_PORT,true, this->BindReuse)) {
	Log(Crit) << "Socket creation (addr=" << *addr << ") on the " << iface->getName() 
		  << "/" << iface->getID() << " interface failed." << LogEnd;
	return false;
    }

    if (llAddr) {
	    char buf[48];
	    inet_ntop6(llAddr,buf);
	    Log(Info) << "Socket bound to " << buf << "/port=" << DHCPCLIENT_PORT << LogEnd;
	    this->ctrlIface = realIface->getID();
	    memcpy(this->ctrlAddr,buf,48);
    } 
    return true;
}

/** this function removes expired addresses from interface and from database
 *  It must be called before AddrMgr::doDuties() is called.
 */
void TClntTransMgr::removeExpired() {

    if (AddrMgr->getValidTimeout())
        return;

    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TAddrAddr> ptrAddr;
    SmartPtr<TIfaceIface> ptrIface;

    this->AddrMgr->firstIA();
    while (ptrIA = this->AddrMgr->getIA()) {
	if (ptrIA->getValidTimeout())
	    continue;

	ptrIA->firstAddr();
	while (ptrAddr = ptrIA->getAddr()) {
	    if (ptrAddr->getValidTimeout())
		continue;
	    ptrIface = this->IfaceMgr->getIfaceByID(ptrIA->getIface());
	    Log(Warning) << "Address " << ptrAddr->get()->getPlain() << " assigned to the "
			 << ptrIface->getName() << "/" << ptrIface->getID() 
			 << " interface (in IA " << ptrIA->getIAID() <<") has expired." << LogEnd;

	    // remove that address from the physical interace
	    ptrIface->delAddr(ptrAddr->get());
	}
    }
}

void TClntTransMgr::checkDB()
{
    SmartPtr <TAddrIA> ptrIA;
    AddrMgr->firstIA();
    AddrMgr->doDuties();
    while ( ptrIA = AddrMgr->getIA()) {
        SmartPtr<TIfaceIface> ptrIface;
        ptrIface = IfaceMgr->getIfaceByID( ptrIA->getIface() );
        if (!ptrIface) {
            // IA with non-existent iface, purge iface
            SmartPtr<TAddrAddr> ptrAddr;
            ptrIA->firstAddr();
            while (ptrAddr = ptrIA->getAddr())
                ptrIA->delAddr( ptrAddr->get() );
            ptrIA->setState(NOTCONFIGURED);
        }
    }
}

bool TClntTransMgr::openLoopbackSocket() {
    SmartPtr<TIfaceIface> ptrIface;

    if (!this->BindReuse)
	return true;

#ifndef WIN32
    SmartPtr<TIfaceIface> loopback;
    IfaceMgr->firstIface();
    while (ptrIface=IfaceMgr->getIface()) {
        if (!ptrIface->flagLoopback()) {
            continue;
	    }
	    loopback = ptrIface;
	    break;
    }
    if (!loopback) {
	   Log(Crit) << "Loopback interface not found!" << LogEnd;
	   return false;
    }

    SmartPtr<TIPv6Addr> loopAddr = new TIPv6Addr("::", true);
    Log(Notice) << "Creating control (" << *loopAddr << ") socket on the " << loopback->getName() 
		<< "/" << loopback->getID() << " interface." << LogEnd;

    if (!loopback->addSocket(loopAddr,DHCPCLIENT_PORT, false, true)) {
	Log(Crit) << "Proper socket creation failed." << LogEnd;
	return false;
    }
#endif
    return true;
}

/*
 * this method is called, when no message has been received, but some
 * action should be taken, e.g. RENEW transmission
 */
void TClntTransMgr::doDuties()
{
    // for each message on list, let it do its duties, if timeout is reached
    SmartPtr <TClntMsg> msg;
    Transactions.first();
    while(msg=Transactions.get())
    {
        if ((!msg->getTimeout())&&(!msg->isDone())) {
	    Log(Info) << "Processing msg (" << msg->getName() << ",transID=0x"
	     << hex << msg->getTransID() << dec << ",opts:";
	    SmartPtr<TOpt> ptrOpt;
	    msg->firstOption();
	    while (ptrOpt = msg->getOption()) {
		Log(Cont) << " " << ptrOpt->getOptType();
	    }
	    Log(Cont) << ")" << LogEnd;
            msg->doDuties();
	}
    }

    // now delete messages which are marked as done
    Transactions.first();
    while (msg = Transactions.get() ) {
        if (msg->isDone())
            Transactions.del();
    }

    this->removeExpired();
    this->AddrMgr->doDuties();

    this->AddrMgr->dump();
    this->IfaceMgr->dump();
    this->CfgMgr->dump();

    if (!this->Shutdown && !this->IsDone) {
        // are there any tentative addrs?
        checkDecline();

        // are there any IAs or TAs to configure?
        checkSolicit();

        //is there any IA in Address manager, which has not sufficient number 
        //of addresses
        checkRequest();

        // are there any aging IAs?
        checkRenew();

        // did we switched links lately?
	// FIXME: for now, we cannot check link switches, so checkConfirm
	// is run only during startup
        //checkConfirm();        

        //Maybe we require only infromations concernig link
        checkInfRequest();
    } 
    
    // This method launch the DNS update, so the checkDecline has to be done before to ensure the ip address is valid
    this->IfaceMgr->doDuties();

    if (this->Shutdown && !Transactions.count())
        this->IsDone = true;
}

void TClntTransMgr::shutdown()
{
    this->Shutdown = true;

    Log(Notice) << "Shutting down entire client." << LogEnd;

    // delete all transactions
    Transactions.clear();

    List(TAddrIA) releasedIAs;

    SmartPtr<TAddrIA> ptrFirstIA;
    SmartPtr<TAddrIA> ptrNextIA;

    // delete all weird-state and address-free IAs 
    AddrMgr->firstIA();
    while (ptrFirstIA = AddrMgr->getIA()) {
        if ( (ptrFirstIA->getState() != CONFIGURED && ptrFirstIA->getState() != INPROCESS) ||
            !ptrFirstIA->countAddr() )
            AddrMgr->delIA(ptrFirstIA->getIAID()); 
    }

    // normal IAs are to be released
    while (AddrMgr->countIA()) {
        // clear the list
        releasedIAs.clear();

        // get first IA
        AddrMgr->firstIA();
        ptrFirstIA = AddrMgr->getIA();
        releasedIAs.append(ptrFirstIA);
        AddrMgr->delIA( ptrFirstIA->getIAID() );

        // find similar IAs 
        while (ptrNextIA = AddrMgr->getIA()) {
            if ((*(ptrFirstIA->getDUID())==*(ptrNextIA->getDUID())) &&
                (ptrFirstIA->getIface() == ptrNextIA->getIface() ) ) {
                    // IA serviced via this same server, add it do the list
                    releasedIAs.append(ptrNextIA);

                    // delete addressess from IfaceMgr
                    SmartPtr<TAddrAddr> ptrAddr;
                    SmartPtr<TIfaceIface> ptrIface;
                    ptrIface = IfaceMgr->getIfaceByID(ptrNextIA->getIface());
                    if (!ptrIface) {
			Log(Error) << "Unable to find " << ptrNextIA->getIface()
                            << " interface while releasing address." << LogEnd;
                        break;
                    }
                    ptrNextIA->firstAddr();

		    // addrs are RELEASEd in ClntMsgRelease.cpp
                    // while (ptrAddr = ptrNextIA->getAddr() ) {
                    //     ptrIface->delAddr( ptrAddr->get() );
                    //}

                    // delete IA from AddrMgr
                    AddrMgr->delIA( ptrNextIA->getIAID() );
                }
        }
        if (releasedIAs.count()) 
            this->sendRelease(releasedIAs);
    }
    
    //doDuties(); // just to send RELEASE msg
    //Transactions.clear(); // delete all transactions

    // clean up options
    this->IfaceMgr->removeAllOpts();
}

void TClntTransMgr::relayMsg(SmartPtr<TClntMsg>  msgAnswer)
{
    SmartPtr<TIfaceIface> ifaceQuestion;
    SmartPtr<TIfaceIface> ifaceAnswer;

    // is message valid?
    if (!msgAnswer->check())
        return ;
    
    // find which message this is answer for
    bool found = false;
    SmartPtr<TClntMsg> msgQuestion;
    Transactions.first();
    while(msgQuestion=(Ptr*)Transactions.get()) {
        if (msgQuestion->getTransID()==msgAnswer->getTransID()) {
            found =true;
	    if (msgQuestion->getIface()!=msgAnswer->getIface()) {
		ifaceQuestion = this->IfaceMgr->getIfaceByID(msgQuestion->getIface());
		ifaceAnswer   = this->IfaceMgr->getIfaceByID(msgAnswer->getIface());
		Log(Warning) << "Reply for transaction 0x" << hex << msgQuestion->getTransID() << dec
			     << " sent on " << ifaceQuestion->getFullName() << " was received on interface " 
			     << ifaceAnswer->getFullName() << "." << LogEnd;
		// return; // don't return, just fix interface ID
		msgAnswer->setIface(msgQuestion->getIface());
	    }

            msgQuestion->answer(msgAnswer);
            break;
        }
    }

    if (!found) 
        Log(Warning) << "Message with wrong transID (" << hex << msgAnswer->getTransID() << dec
		     << ") received. Ignoring." << LogEnd;
    AddrMgr->dump();
}

unsigned long TClntTransMgr::getTimeout()
{
    unsigned long timeout = DHCPV6_INFINITY;
    unsigned long tmp;

    if (this->IsDone) 
        return 0;

    // AddrMgr timeout
    timeout = AddrMgr->getTimeout();
    tmp     = AddrMgr->getTentativeTimeout();
    if (timeout > tmp)
        timeout = tmp;

    // IfaceMgr (Lifetime option) timeout
    tmp = IfaceMgr->getTimeout();
    if (timeout > tmp)
	timeout = tmp;

    // Messages timeout
    SmartPtr<TClntMsg> ptrMsg;
    Transactions.first();
    tmp = DHCPV6_INFINITY;
    while(ptrMsg=Transactions.get())
    {
        if (tmp > ptrMsg->getTimeout())
            tmp = ptrMsg->getTimeout();
    }
    if (timeout > tmp)
	timeout = tmp;

    return timeout;
}

void TClntTransMgr::stop()
{
}

/**
 * Note: requestOptions list MUST NOT contain server DUID.
 */
void TClntTransMgr::sendRequest(List(TOpt) requestOptions, 
                                List(TMsg) srvlist,int iface)
{
    SmartPtr<TOpt> opt;
    requestOptions.first();
    while (opt = requestOptions.get()) {
	if (!allowOptInMsg(REQUEST_MSG, opt->getOptType()))
	    requestOptions.del();
    }
    SmartPtr<TClntMsg> ptr = new TClntMsgRequest(IfaceMgr,That,CfgMgr, AddrMgr, requestOptions,srvlist,iface);
    Transactions.append( ptr );
}

// Send RELEASE message
void TClntTransMgr::sendRelease( List(TAddrIA) IALst)
{
    if (!IALst.count()) {
        Log(Error) << "Unable to send RELEASE with empty IAs list." << LogEnd;
        return;
    }

    SmartPtr<TAddrIA> ptrIA;
    IALst.first();
    ptrIA = IALst.get();
    Log(Notice) << "Creating RELEASE for " << IALst.count() << " IA(s)." << LogEnd;

    SmartPtr<TClntMsg> ptr = new TClntMsgRelease(IfaceMgr,That,CfgMgr, AddrMgr, ptrIA->getIface(), 
        ptrIA->getSrvAddr(), IALst);
    Transactions.append( ptr );
}

// Send REBIND message
void TClntTransMgr::sendRebind(List(TOpt) requestOptions, int iface) {
    SmartPtr<TOpt> opt;
    requestOptions.first();
    while (opt = requestOptions.get()) {
	if (!allowOptInMsg(REBIND_MSG, opt->getOptType()))
	    requestOptions.del();
    }

    SmartPtr<TClntMsg> ptr =  new TClntMsgRebind(IfaceMgr, That, CfgMgr, AddrMgr, requestOptions, iface);
    if (!ptr->isDone())
	Transactions.append( ptr );
}

void TClntTransMgr::sendInfRequest(List(TOpt) requestOptions, int iface) {
    SmartPtr<TOpt> opt;
    requestOptions.first();
    while (opt = requestOptions.get()) {
	if (!allowOptInMsg(INFORMATION_REQUEST_MSG, opt->getOptType()))
	    requestOptions.del();
    }

    SmartPtr<TClntMsg> ptr = new TClntMsgInfRequest(IfaceMgr,That,CfgMgr,AddrMgr,requestOptions,iface);
    if (!ptr->isDone())
	Transactions.append( ptr );    
}

// should we send SOLICIT ?
void TClntTransMgr::checkSolicit() {

    //For every iface, every group in iface in ClntCfgMgr 
    //Enumerate IA's from this group
    SmartPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while( (iface=CfgMgr->getIface()) )
    {
        if (iface->noConfig())
            continue;

	// step 1: check if there are any IA to be configured
	List(TClntCfgIA) iaLstToConfig; // list of IA requiring configuration
	SmartPtr<TClntCfgIA> cfgIA;
    
	iface->firstIA();
	while(cfgIA=iface->getIA())
	{
	    //These not assigned in AddrMgr and configurable and not in trasaction
	    //group and pass to constructor of Solicit message
	    SmartPtr<TAddrIA> ia=AddrMgr->getIA(cfgIA->getIAID());
	    if(ia->getState()==NOTCONFIGURED)
	    {
		iaLstToConfig.append(cfgIA);
		ia->setState(INPROCESS);
	    }
	}

	// step 2: check if TA has to be configured
	bool taToConfig = false;
	if (iface->countTA() ) {
	    iface->firstTA();
	    SmartPtr<TClntCfgTA> ta = iface->getTA();
	    if (ta->getState()==NOTCONFIGURED) {
		taToConfig = true;
		// don't change set to INPROCESS, this will be done in ClntMsg.cpp, when
		// TA option will be appended.
	    }
	}

	if (iaLstToConfig.count() || taToConfig) {//Are there any IA, which should be configured?
	    Log(Info) << "Creating SOLICIT message with " << iaLstToConfig.count()
		      << " IA(s), " << (iface->countTA()?"1":"no") << " TA ";
	    if (iface->getRapidCommit()) {
		Log(Cont) << "(with rapid-commit)";
	    } 
	    Log(Cont) << " on " << iface->getName() <<" interface." << LogEnd;
	    Transactions.append(new TClntMsgSolicit(IfaceMgr,That,CfgMgr,AddrMgr,
						    iface->getID(), 0, iaLstToConfig, iface->getRapidCommit()));
        }//for every group
    }//for every iface
}

void TClntTransMgr::checkConfirm()
{
    //FIXME: When should we send CONFIRM? How to detect switching to new link?
    //Is it a start of address of manager
    if(!ConfirmEnabled)
        return;
    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TIfaceIface> ptrIface;
    IfaceMgr->firstIface();
    while(ptrIface=IfaceMgr->getIface())
    {
        TContainer<SmartPtr<TAddrIA> > IALst;
        AddrMgr->firstIA();
        while(ptrIA=AddrMgr->getIA())
        {
            if(ptrIA->getIface()==ptrIface->getID())
            {
                IALst.append(ptrIA);
                ptrIA->setState(INPROCESS);
            }
        }
        if (IALst.count())
            Transactions.append(
            new TClntMsgConfirm(IfaceMgr,That,CfgMgr,AddrMgr,ptrIface->getID(),IALst));
    }
}

void TClntTransMgr::checkInfRequest()
{
    SmartPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while( (iface=CfgMgr->getIface()) )
    {
        if (iface->noConfig())
            continue;
	SmartPtr<TClntIfaceIface> ifaceIface = (Ptr*)IfaceMgr->getIfaceByID(iface->getID());
	if (!ifaceIface) {
	    Log(Error) << "Interface with ifindex=" << iface->getID() << " not found." << LogEnd;
	    continue;
	}
	if (!ifaceIface->getTimeout()) {
	    if (iface->getDNSServerState()  == CONFIGURED) 
		iface->setDNSServerState(NOTCONFIGURED);
	    if (iface->getDomainState()     == CONFIGURED) 
		iface->setDomainState(NOTCONFIGURED);
	    if (iface->getNTPServerState()  == CONFIGURED) 
		iface->setNTPServerState(NOTCONFIGURED);
            if (iface->getTimezoneState()   == CONFIGURED) 
		iface->setTimezoneState (NOTCONFIGURED);
	    if (iface->getSIPServerState()  == CONFIGURED) 
		iface->setSIPServerState(NOTCONFIGURED);
	    if (iface->getSIPDomainState()  == CONFIGURED) 
		iface->setSIPDomainState(NOTCONFIGURED);
	    if (iface->getFQDNState()       == CONFIGURED) 
		iface->setFQDNState(NOTCONFIGURED);
	    if (iface->getNISServerState()  == CONFIGURED) 
		iface->setNISServerState(NOTCONFIGURED);
	    if (iface->getNISDomainState()  == CONFIGURED) 
		iface->setNISDomainState(NOTCONFIGURED);
	    if (iface->getNISPServerState() == CONFIGURED) 
		iface->setNISPServerState(NOTCONFIGURED);
	    if (iface->getNISPDomainState() == CONFIGURED) 
		iface->setNISPDomainState(NOTCONFIGURED);
	}

	if ( (iface->getDNSServerState()  == NOTCONFIGURED) ||
	     (iface->getDomainState()     == NOTCONFIGURED) ||
	     (iface->getNTPServerState()  == NOTCONFIGURED) ||
	     (iface->getTimezoneState()   == NOTCONFIGURED) ||
	     (iface->getSIPServerState()  == NOTCONFIGURED) ||
	     (iface->getSIPDomainState()  == NOTCONFIGURED) ||
	     (iface->getFQDNState()       == NOTCONFIGURED) ||
	     (iface->getNISServerState()  == NOTCONFIGURED) ||
	     (iface->getNISDomainState()  == NOTCONFIGURED) ||
	     (iface->getNISPServerState() == NOTCONFIGURED) ||
	     (iface->getNISPDomainState() == NOTCONFIGURED) ) {
	    Log(Info) << "Creating INFORMATION-REQUEST message on "
		      << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
            Transactions.append(new TClntMsgInfRequest(IfaceMgr,That,CfgMgr,AddrMgr,iface));
	}
    }
}

void TClntTransMgr::checkRenew()
{
    // are there any IAs which require RENEW?
    if (AddrMgr->getT1Timeout() > 0 ) 
	return;

    // yes, there are. Find them!
    AddrMgr->firstIA();
    SmartPtr < TAddrIA> ptrIA;
    while (ptrIA = AddrMgr->getIA() ) 
    {
        if ( (ptrIA->getT1Timeout() == 0) && (ptrIA->getState()==CONFIGURED) ) 
        {
            // to avoid race conditions (RENEW vs DECLINE)
            if (ptrIA->getTentative()==DONTKNOWYET)
                continue;

            TContainer<SmartPtr<TAddrIA> > iaLst;
            Log(Notice) << "IA (IAID=" << ptrIA->getIAID() 
                << ") needs RENEW. Trying to group with other IA(s) requiring renewal:";
            SmartPtr<TAddrIA> iaPattern=ptrIA;
            iaLst.append(ptrIA);
            ptrIA->setState(INPROCESS);
            while(ptrIA = AddrMgr->getIA())
            {
                //the same diffrence between T1(which has just elapsed
                //for both IAs) and T2
                if (((ptrIA->getT2()-ptrIA->getT1())==
                    (iaPattern->getT2()-iaPattern->getT1()))&&
                    (ptrIA->getIface()==iaPattern->getIface())&&
                    (ptrIA->getDUID()==iaPattern->getDUID()))
                {
		    Log(Cont) << ptrIA->getIAID() << " ";
                    iaLst.append(ptrIA);
                    ptrIA->setState(INPROCESS);
                }
            }
	    if (iaLst.count()==1) {
		Log(Cont) << "none found." << LogEnd;
	    } else {
		Log(Cont) << "." << LogEnd;
	    }
	    
            SmartPtr <TClntMsg> ptrRenew = new TClntMsgRenew(IfaceMgr, That, CfgMgr, AddrMgr, iaLst);
            Transactions.append(ptrRenew);
            AddrMgr->firstIA();
        }
    }
}

void TClntTransMgr::checkDecline()
{
    //Find any tentative address and remove them from address manager
    //Group declined addresses and IAs by server and send Decline
    //Find first IA with tentative addresses
    SmartPtr<TAddrIA> firstIA;
    SmartPtr<TAddrIA> ptrIA;
    int result;
    do
    {
        firstIA=SmartPtr<TAddrIA>(); // NULL
        TContainer<SmartPtr<TAddrIA> > declineIALst;

        AddrMgr->firstIA();
        while((ptrIA=AddrMgr->getIA())&&(!firstIA))
        {
            if (ptrIA->getTentative()==YES)
                firstIA=ptrIA;
        }
        if (firstIA)
        {
            declineIALst.append(firstIA);
            while(ptrIA=AddrMgr->getIA())
            {
                if ((ptrIA->getTentative()==YES)&&
                    (*ptrIA->getDUID()==*firstIA->getDUID()))
                {
                    declineIALst.append(ptrIA);
                }

            }
            //Here should be send decline for all tentative addresses in IAs
            SmartPtr<TClntMsgDecline> decline = 
                new TClntMsgDecline(IfaceMgr, That, CfgMgr, AddrMgr,
                firstIA->getIface(), SmartPtr<TIPv6Addr>(),
                declineIALst);
            Transactions.append( (Ptr*) decline);

            // decline sent, now remove those addrs from IfaceMgr
            SmartPtr<TIfaceIface> ptrIface = IfaceMgr->getIfaceByID(firstIA->getIface());

            declineIALst.first();
            while (ptrIA = declineIALst.get() ) {
                SmartPtr<TAddrAddr> ptrAddr;
                ptrIA->firstAddr();
		Log(Info) << "Sending DECLINE for IA(IAID=" << ptrIA->getIAID() << "): ";

                while ( ptrAddr= ptrIA->getAddr() ) {
                    if (ptrAddr->getTentative() == YES) {
                        // remove this address from interface
                        Log(Cont) << "(" << ptrAddr->get()->getPlain();
                        result = ptrIface->delAddr(ptrAddr->get());
                        Log(Cont) << " Iface removal=" << result;

                        // remove this address from addrDB
                        result = ptrIA->delAddr(ptrAddr->get());
                        Log(Cont) << " AddrDB removal=" << result << ")";
                    }
                }
		Log(Cont) << LogEnd;
		
                ptrIA->setTentative();
            }
        }
    } while(firstIA);
}

void TClntTransMgr::checkRequest()
{
    SmartPtr<TAddrIA> ia;
    SmartPtr<TClntCfgIA> cfgIA;
    SmartPtr<TDUID> duid = 0;
    List(TAddrIA) requestIALst;
    int ifaceID = 0;

    requestIALst.clear();
    AddrMgr->firstIA();
    while( ia=AddrMgr->getIA() ) {

	cfgIA = CfgMgr->getIA(ia->getIAID());
	if (!cfgIA) {
	    Log(Error) << "Internal sanity check failed. Unable to find IA (iaid=" << ia->getIAID()
		       << ") in the CfgMgr." << LogEnd;
	    continue;
	}

	// find all IAs which should be configured:
	if ( (ia->getState()!=NOTCONFIGURED) )
	    continue;

	// this IA is being serviced by different server
	if ( (duid) && !((*ia->getDUID()) == (*duid)) ) {
	    continue;
	}

	// if we have found one IA, following ones must be on the same interface
	if ( ifaceID && (ia->getIface()!=ifaceID) ) {
	    continue;
	}

	if ( ia->countAddr() < cfgIA->countAddr() ) {
	    Log(Info) << "IA (iaid=" << ia->getIAID() << ") is configured to get " 
		      << cfgIA->countAddr() << " addr, but server provided " << ia->countAddr() 
		      << ", REQUEST will be sent." << LogEnd;
	}

	requestIALst.append(ia);
	ia->setState(INPROCESS);
	duid = ia->getDUID();
	ifaceID = ia->getIface();
    }

    if (requestIALst.count()) {
	// create REQUEST message
	SmartPtr<TClntMsgRequest> request;
	request = new TClntMsgRequest(IfaceMgr, That, CfgMgr, AddrMgr,
				      requestIALst, duid, ifaceID );
	Transactions.append( (Ptr*) request);
    } 
}

bool TClntTransMgr::isDone()
{
    return IsDone;
}

void TClntTransMgr::setThat(SmartPtr<TClntTransMgr> that)
{
    this->That=that;
    IfaceMgr->setThats(IfaceMgr,That,CfgMgr,AddrMgr);
}

char* TClntTransMgr::getCtrlAddr() {
	return this->ctrlAddr;
}
int  TClntTransMgr::getCtrlIface() {
	return this->ctrlIface;
}

TClntTransMgr::~TClntTransMgr() {
    Log(Debug) << "ClntTransMgr cleanup." << LogEnd;
}
