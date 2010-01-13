/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntTransMgr.cpp,v 1.62 2008-11-13 22:18:17 thomson Exp $
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
#include "ClntCfgPD.h"
#include "Msg.h"
#include "ClntMsgAdvertise.h"
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

TClntTransMgr::TClntTransMgr(SPtr<TClntIfaceMgr> ifaceMgr, 
			     SPtr<TClntAddrMgr> addrMgr,
			     SPtr<TClntCfgMgr> cfgMgr,
                             string config)
{
    // should we set REUSE option during binding sockets?
#ifdef MOD_CLNT_BIND_REUSE
    BindReuse = true;
#else
    BindReuse = false;
#endif

    IsDone = true;

    IfaceMgr = ifaceMgr;
    AddrMgr  = addrMgr;
    CfgMgr   = cfgMgr;

    if (this->BindReuse)
	Log(Debug) << "Bind reuse enabled (multiple instances allowed)." << LogEnd;
    else
	Log(Debug) << "Bind reuse disabled (multiple instances not allowed)." << LogEnd;

    if (!this->openLoopbackSocket()) {
	return;
    }

    SPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while(iface=CfgMgr->getIface()) {
	if (!this->openSocket(iface)) {
	    return;
	}
    }

    this->checkDB();
    this->Shutdown = false;
    this->IsDone = false;
}

bool TClntTransMgr::openSocket(SPtr<TClntCfgIface> iface) {

    if (iface->noConfig())       
	return true;

    // create IAs in AddrMgr corresponding to those specified in CfgMgr.
    SPtr<TClntCfgIA> ia;
    iface->firstIA();
    while(ia=iface->getIA()) {
	AddrMgr->addIA(new TAddrIA(iface->getID(),
				   TAddrIA::TYPE_IA,
				   SPtr<TIPv6Addr>(), 
				   SPtr<TDUID>(),CLIENT_DEFAULT_T1,CLIENT_DEFAULT_T2,
				   ia->getIAID()));
    }
    
    // open socket
    SPtr<TIfaceIface> realIface = IfaceMgr->getIfaceByID(iface->getID());
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

    SPtr<TIPv6Addr> addr = new TIPv6Addr(llAddr);

#if 0
#ifndef WIN32
    SPtr<TIfaceIface> loopback;
    SPtr<TIfaceIface> ptrIface;
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

    SPtr<TAddrIA> ptrIA;
    SPtr<TAddrAddr> ptrAddr;
    SPtr<TIfaceIface> ptrIface;

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
	    ptrIface->delAddr(ptrAddr->get(), ptrIface->getPrefixLength());
	}
    }
}

/** 
 * checks if loaded Address database is sanite (i.e. does not reffer to non-existing interface)
 * 
 */
void TClntTransMgr::checkDB()
{
    SPtr<TClntCfgIface> iface;
    SPtr <TAddrIA> ptrIA;
    SPtr<TAddrAddr> ptrAddr;

    AddrMgr->doDuties();
    AddrMgr->firstIA();
    while ( ptrIA = AddrMgr->getIA()) {
        iface = CfgMgr->getIface( ptrIA->getIface() );
        if (!iface) {
	    Log(Warning) << "IA (iaid=" << ptrIA->getIAID() << ") loaded from old file, but currently there is no iface with ifindex="
			 << ptrIA->getIface();
            // IA with non-existent iface, purge iface
            ptrIA->firstAddr();
            while (ptrAddr = ptrIA->getAddr())
                ptrIA->delAddr( ptrAddr->get() );
            ptrIA->setState(STATE_NOTCONFIGURED);
	    AddrMgr->delIA(ptrIA->getIAID());
        }
    }
}

bool TClntTransMgr::openLoopbackSocket() {
    SPtr<TIfaceIface> ptrIface;

    if (!this->BindReuse)
	return true;

#ifndef WIN32
    SPtr<TIfaceIface> loopback;
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

    SPtr<TIPv6Addr> loopAddr = new TIPv6Addr("::", true);
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
    SPtr <TClntMsg> msg;
    Transactions.first();
    while(msg=Transactions.get())
    {
        if ((!msg->getTimeout())&&(!msg->isDone())) {
	    Log(Info) << "Processing msg (" << msg->getName() << ",transID=0x"
	     << hex << msg->getTransID() << dec << ",opts:";
	    SPtr<TOpt> ptrOpt;
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

    if (CfgMgr->inactiveMode())
    {
	SPtr<TClntCfgIface> x;
	x = CfgMgr->checkInactiveIfaces();
	if (x)
	    openSocket(x);
    }

    this->removeExpired();
    this->AddrMgr->doDuties();

    this->AddrMgr->dump();
    this->IfaceMgr->dump();
    this->CfgMgr->dump();

    if (!this->Shutdown && !this->IsDone) {

        // did we switched links lately?
	// are there any IAs to confirm?
	checkConfirm();
	
        // are there any tentative addrs?
        checkDecline();

        // are there any IAs or TAs to configure?
        checkSolicit();

        //is there any IA in Address manager, which has not sufficient number 
        //of addresses
        checkRequest();

        // are there any aging IAs or PDs?
        checkRenew();

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
    SPtr<TAddrIA> ptrFirstIA;
    SPtr<TAddrIA> ptrNextIA;
    SPtr<TAddrIA> ta;
    SPtr<TAddrIA> pd;
    SPtr<TClntCfgIface> iface;
    List(TAddrIA) releasedIAs;
    List(TAddrIA) releasedPDs;

    Transactions.clear(); // delete all transactions
    this->Shutdown = true;
    Log(Notice) << "Shutting down entire client." << LogEnd;
	
    // delete all weird-state/innormal-state and address-free IAs 
    AddrMgr->firstIA();
    while (ptrFirstIA = AddrMgr->getIA()) {
        if ( (ptrFirstIA->getState() != STATE_CONFIGURED && ptrFirstIA->getState() != STATE_INPROCESS) ||
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
                (ptrFirstIA->getIface() == ptrNextIA->getIface() ) ) 
            {
                // IA serviced via this same server, add it do the list
                releasedIAs.append(ptrNextIA);

                // delete addressess from IfaceMgr
                SPtr<TAddrAddr> ptrAddr;
                SPtr<TIfaceIface> ptrIface;
                ptrIface = IfaceMgr->getIfaceByID(ptrNextIA->getIface());
                if (!ptrIface) {
		            Log(Error) << "Unable to find " << ptrNextIA->getIface()
                               << " interface while releasing address." << LogEnd;
                    break;
                }
                ptrNextIA->firstAddr();

    		    // addresses are deleted in ClntMsgRelease.cpp, DO NOT delete them here
                // while (ptrAddr = ptrNextIA->getAddr() ) {
                //    ptrIface->delAddr( ptrAddr->get(),ptrAddr->getPrefix() );
                // }

                // delete IA from AddrMgr
                AddrMgr->delIA( ptrNextIA->getIAID() );
            }
        }

    	ta = 0;
        if (releasedIAs.count()) { 
	        // check if there are TA to release
	        releasedIAs.first();
	        iface = CfgMgr->getIface(releasedIAs.get()->getIface());
	        if (iface && iface->countTA()) {
    		    iface->firstTA();
	    	    SPtr<TClntCfgTA> cfgTA = iface->getTA();
		        ta = AddrMgr->getTA(cfgTA->getIAID());
		        cfgTA->setState(STATE_DISABLED);
	        }

    	}
	    pd = 0;
	    AddrMgr->firstPD();
	    while (pd = AddrMgr->getPD()) {
	        releasedPDs.append(pd);
	        SPtr<TClntCfgPD> cfgPD = CfgMgr->getPD(pd->getIAID());
	        if (cfgPD)
		    cfgPD->setState(STATE_DISABLED);
	    }
	    this->sendRelease(releasedIAs,ta, releasedPDs);
    }

    // now check if there are any TA left
    CfgMgr->firstIface();
    while (iface = CfgMgr->getIface()) {
	if (iface->countTA()) {
	    iface->firstTA();
	    SPtr<TClntCfgTA> cfgTA = iface->getTA();
	    ta = AddrMgr->getTA(cfgTA->getIAID());
	    releasedIAs.clear();
	    if (!ta) {
		Log(Warning) << "Unable to find TA(taid=" << cfgTA->getIAID() <<"). " << LogEnd;
		continue;
	    }
	    if (cfgTA->getState()==STATE_CONFIGURED) {
		this->sendRelease(releasedIAs, ta, releasedPDs);
		cfgTA->setState(STATE_DISABLED);
	    }
	}

    }

    // are there any PDs left to release?
    pd = 0;
    AddrMgr->firstPD();
    releasedIAs.clear();
    releasedPDs.clear();
    while (pd = AddrMgr->getPD()) {
	releasedPDs.append(pd);
	SPtr<TClntCfgPD> cfgPD = CfgMgr->getPD(pd->getIAID());
	if (cfgPD)
	    cfgPD->setState(STATE_DISABLED);
    }
    if (releasedPDs.count())
	this->sendRelease(releasedIAs, 0, releasedPDs);
    
    //CHANGED:the following two lines are uncommented.
    doDuties(); // just to send RELEASE msg
    Transactions.clear(); // delete all transactions

    // clean up options
    this->IfaceMgr->removeAllOpts();
}

void TClntTransMgr::relayMsg(SPtr<TClntMsg>  msgAnswer)
{
    SPtr<TIfaceIface> ifaceQuestion;
    SPtr<TIfaceIface> ifaceAnswer;

    // is message valid?
    if (!msgAnswer->check())
        return ;
    
    // find which message this is answer for
    bool found = false;
    SPtr<TClntMsg> msgQuestion;
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
    {
	if (!Shutdown)
	    Log(Warning) << "Message with wrong transID (0x" << hex << msgAnswer->getTransID() << dec
			 << ") received. Ignoring." << LogEnd;
	else
	    Log(Debug) << "Message with transID=0x" << hex << msgAnswer->getTransID() << dec
		       << " received, but ignored during shutdown." << LogEnd;
    } 
    CfgMgr->dump();
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
    SPtr<TClntMsg> ptrMsg;
    Transactions.first();
    tmp = DHCPV6_INFINITY;
    while(ptrMsg=Transactions.get())
    {
        if (tmp > ptrMsg->getTimeout())
            tmp = ptrMsg->getTimeout();
    }
    if (timeout > tmp)
	timeout = tmp;

    if (CfgMgr->inactiveIfacesCnt()) {
	if (timeout>INACTIVE_MODE_INTERVAL)
	    timeout=INACTIVE_MODE_INTERVAL;
    }

    return timeout;
}

void TClntTransMgr::stop()
{
}

/**
 * Note: requestOptions list MUST NOT contain server DUID.
 */
void TClntTransMgr::sendRequest(List(TOpt) requestOptions, int iface)
{
    sortAdvertiseLst();

    SPtr<TOpt> opt;
    requestOptions.first();
    while (opt = requestOptions.get()) {
	if (!allowOptInMsg(REQUEST_MSG, opt->getOptType()))
	    requestOptions.del();
    }
    SPtr<TClntMsg> ptr = new TClntMsgRequest(IfaceMgr,That,CfgMgr, AddrMgr, requestOptions, iface);
    Transactions.append( (Ptr*)ptr );
}

// Send RELEASE message
void TClntTransMgr::sendRelease( List(TAddrIA) IALst, SPtr<TAddrIA> ta, List(TAddrIA) pdLst)
{
    if ( !IALst.count() && !ta && !pdLst.count() ) {
        Log(Error) << "Unable to send RELEASE with empty IA list, PD list and no TA." << LogEnd;
        return;
    }

    // find interface and srv address
    int iface;
    SPtr<TIPv6Addr> addr;
    SPtr<TAddrIA> ptrIA;
    if (IALst.count()) {
	    IALst.first();
	    ptrIA = IALst.get();
	    iface = ptrIA->getIface();
	    addr  = ptrIA->getSrvAddr();
    } else if (pdLst.count()) {
	    pdLst.first();
	    ptrIA = pdLst.get();
	    iface = ptrIA->getIface();
	    addr  = ptrIA->getSrvAddr();
    } else if (ta) {
	    iface = ta->getIface();
	    addr  = ta->getSrvAddr();
    } else {
	    Log(Error) << "Unable to send RELEASE message. No IA, PD or TA defined." << LogEnd;
	    return;
    }

    SPtr<TClntCfgIface> ptrIface = CfgMgr->getIface(iface);
    if (!iface) {
	    Log(Error) << "Unable to find interface with ifindex=" << iface << LogEnd;
	    return;
    }
	
    Log(Notice) << "Creating RELEASE for " << IALst.count() << " IA(s), " 
		<< pdLst.count() << " PD(s), "
		<< (ta?" and TA":" (no TA)") << " on the " << ptrIface->getFullName() 
		<< " interface." << LogEnd;

    SPtr<TClntMsg> ptr = new TClntMsgRelease(IfaceMgr,That,CfgMgr, AddrMgr, iface, 
						 addr, IALst, ta, pdLst);
    Transactions.append( ptr );
}

// Send REBIND message
void TClntTransMgr::sendRebind(List(TOpt) requestOptions, int iface) {
    SPtr<TOpt> opt;
    requestOptions.first();
    while (opt = requestOptions.get()) {
	if (!allowOptInMsg(REBIND_MSG, opt->getOptType()))
	    requestOptions.del();
    }

    SPtr<TClntMsg> ptr =  new TClntMsgRebind(IfaceMgr, That, CfgMgr, AddrMgr, requestOptions, iface);
    if (!ptr->isDone())
	Transactions.append( ptr );
}

void TClntTransMgr::sendInfRequest(List(TOpt) requestOptions, int iface) {
    SPtr<TOpt> opt;
    requestOptions.first();
    while (opt = requestOptions.get()) {
	if (!allowOptInMsg(INFORMATION_REQUEST_MSG, opt->getOptType()))
	    requestOptions.del();
    }

    SPtr<TClntMsg> ptr = new TClntMsgInfRequest(IfaceMgr,That,CfgMgr,AddrMgr,requestOptions,iface);
    if (!ptr->isDone())
	Transactions.append( ptr );    
}

// should we send SOLICIT ?
void TClntTransMgr::checkSolicit() {

    //For every iface, every group in iface in ClntCfgMgr 
    //Enumerate IA's from this group
    SPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while( (iface=CfgMgr->getIface()) )
    {
        if (iface->noConfig())
            continue;

	// step 1: check if there are any IA to be configured
	List(TClntCfgIA) iaLst; // list of IA requiring configuration
	SPtr<TClntCfgIA> cfgIA;
    
	iface->firstIA();
	while(cfgIA=iface->getIA())
	{
	    // These not assigned in AddrMgr and configurable and not in 
	    // trasaction group and pass to constructor of Solicit message
	    if(cfgIA->getState()==STATE_NOTCONFIGURED)
		iaLst.append(cfgIA);
	}

	// step 2: check if TA has to be configured
	SPtr<TClntCfgTA> ta;
	if (iface->countTA() ) {
	    iface->firstTA();
	    ta = iface->getTA();
	    if (ta->getState()==STATE_NOTCONFIGURED)
		ta->setState(STATE_INPROCESS);
	    else
		ta = 0;
	}

	// step 3: check if there are any PD to be configured
	List(TClntCfgPD) pdLst;
	SPtr<TClntCfgPD> pd;

	iface->firstPD();
	while ( pd = iface->getPD() ) {
	    if (pd->getState()==STATE_NOTCONFIGURED) {
		pdLst.append(pd);
	    }
	}

	//Are there any IA, TA or PD which should be configured?
	if (iaLst.count() || ta || pdLst.count()) {
	    Log(Info) << "Creating SOLICIT message with " << iaLst.count()
		      << " IA(s), " << (ta?"1":"no") << " TA and " << pdLst.count()
		      << " PD(s)";
	    if (iface->getRapidCommit()) {
		Log(Cont) << " (with rapid-commit)";
	    } 
	    Log(Cont) << " on " << iface->getFullName() <<" interface." << LogEnd;
	    Transactions.append(new TClntMsgSolicit(IfaceMgr,That,CfgMgr,AddrMgr,
						    iface->getID(), 0, iaLst, ta, pdLst, 
						    iface->getRapidCommit()));
        }

    }//for every iface
}

void TClntTransMgr::checkConfirm()
{
    SPtr<TAddrIA> ptrIA;
    SPtr<TClntCfgIface> iface;
    SPtr<TClntCfgIA> cfgIA;
    List(TAddrIA) IALst;

    CfgMgr->firstIface();
    while(iface=CfgMgr->getIface())
    {
	IALst.clear();
	iface->firstIA();
	while (cfgIA = iface->getIA()) {
	    AddrMgr->firstIA();
	    while(ptrIA=AddrMgr->getIA())
	    {
		if (ptrIA->getState()!=STATE_CONFIRMME)	
		    continue;
		if (ptrIA->getIface()==iface->getID())
		{
		    IALst.append(ptrIA);
		    ptrIA->setState(STATE_INPROCESS);
		    cfgIA->setState(STATE_INPROCESS);
		}
	    }
	}	    

        if (IALst.count()) {
	    Log(Info) << "Creating CONFIRM: " << IALst.count() << " IA(s) on " << iface->getFullName() << LogEnd;
            Transactions.append(
		new TClntMsgConfirm(IfaceMgr,That,CfgMgr,AddrMgr,iface->getID(),IALst));
	}
    }
}

void TClntTransMgr::checkInfRequest()
{
    SPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while( (iface=CfgMgr->getIface()) )
    {
        if (iface->noConfig())
            continue;
	SPtr<TClntIfaceIface> ifaceIface = (Ptr*)IfaceMgr->getIfaceByID(iface->getID());
	if (!ifaceIface) {
	    Log(Error) << "Interface with ifindex=" << iface->getID() << " not found." << LogEnd;
	    continue;
	}
	if (!ifaceIface->getTimeout()) {
	    if (iface->getDNSServerState()  == STATE_CONFIGURED) 
		iface->setDNSServerState(STATE_NOTCONFIGURED);
	    if (iface->getDomainState()     == STATE_CONFIGURED) 
		iface->setDomainState(STATE_NOTCONFIGURED);
	    if (iface->getNTPServerState()  == STATE_CONFIGURED) 
		iface->setNTPServerState(STATE_NOTCONFIGURED);
            if (iface->getTimezoneState()   == STATE_CONFIGURED) 
		iface->setTimezoneState (STATE_NOTCONFIGURED);
	    if (iface->getSIPServerState()  == STATE_CONFIGURED) 
		iface->setSIPServerState(STATE_NOTCONFIGURED);
	    if (iface->getSIPDomainState()  == STATE_CONFIGURED) 
		iface->setSIPDomainState(STATE_NOTCONFIGURED);
	    if (iface->getFQDNState()       == STATE_CONFIGURED) 
		iface->setFQDNState(STATE_NOTCONFIGURED);
	    if (iface->getNISServerState()  == STATE_CONFIGURED) 
		iface->setNISServerState(STATE_NOTCONFIGURED);
	    if (iface->getNISDomainState()  == STATE_CONFIGURED) 
		iface->setNISDomainState(STATE_NOTCONFIGURED);
	    if (iface->getNISPServerState() == STATE_CONFIGURED) 
		iface->setNISPServerState(STATE_NOTCONFIGURED);
	    if (iface->getNISPDomainState() == STATE_CONFIGURED) 
		iface->setNISPDomainState(STATE_NOTCONFIGURED);
	    if (iface->getKeyGenerationState() == STATE_CONFIGURED) 
		iface->setKeyGenerationState(STATE_NOTCONFIGURED);
	    if (iface->getAuthenticationState() == STATE_CONFIGURED) 
		iface->setAuthenticationState(STATE_NOTCONFIGURED);
	}

	if ( (iface->getDNSServerState()     == STATE_NOTCONFIGURED) ||
	     (iface->getDomainState()        == STATE_NOTCONFIGURED) ||
	     (iface->getNTPServerState()     == STATE_NOTCONFIGURED) ||
	     (iface->getTimezoneState()      == STATE_NOTCONFIGURED) ||
	     (iface->getSIPServerState()     == STATE_NOTCONFIGURED) ||
	     (iface->getSIPDomainState()     == STATE_NOTCONFIGURED) ||
	     (iface->getFQDNState()          == STATE_NOTCONFIGURED) ||
	     (iface->getNISServerState()     == STATE_NOTCONFIGURED) ||
	     (iface->getNISDomainState()     == STATE_NOTCONFIGURED) ||
	     (iface->getNISPServerState()    == STATE_NOTCONFIGURED) ||
	     (iface->getNISPDomainState()    == STATE_NOTCONFIGURED)
         )
        {
	    Log(Info) << "Creating INFORMATION-REQUEST message on "
		      << iface->getFullName() << " interface." << LogEnd;
            Transactions.append(new TClntMsgInfRequest(IfaceMgr,That,CfgMgr,AddrMgr,iface));
	}
    }
}

void TClntTransMgr::checkRenew()
{
    // are there any IAs which require RENEW?
    if (AddrMgr->getT1Timeout() > 0 ) 
	return;

    // TENTATIVE_YES, there are. Find them!
    
    // Find all IAs
    List(TAddrIA) iaLst;
    SPtr<TAddrIA> ia;
    SPtr<TAddrIA> iaPattern;
    AddrMgr->firstIA();

    // Need to be fixed:?? how to deal with mutiple network interfaces.
    while (ia = AddrMgr->getIA() ) 
    {
        if ( (ia->getT1Timeout()!=0) || 
	     (ia->getState()!=STATE_CONFIGURED) ||
	     (ia->getTentative()==TENTATIVE_UNKNOWN) )
	    continue;
	
	if (!iaPattern) {
	    iaPattern = ia;
	    iaLst.append(ia);
	    ia->setState(STATE_INPROCESS);
	    continue;
	}

	if ( (ia->getIface() == iaPattern->getIface()) &&
	     (ia->getDUID()  == iaPattern->getDUID()) )
	{
		iaLst.append(ia);
		ia->setState(STATE_INPROCESS);
	}
    }

    // Find all PDs
    List(TAddrIA) pdLst;
    AddrMgr->firstPD();
    while (ia = AddrMgr->getPD()) {
        if ( (ia->getT1Timeout()!=0) || 
	     (ia->getState()!=STATE_CONFIGURED) )
	    continue;
	
	if (!iaPattern) {
	    iaPattern = ia;
	    pdLst.append(ia);
	    ia->setState(STATE_INPROCESS);
	    continue;
	}

	if ( (ia->getIface() == iaPattern->getIface()) &&
	     (ia->getDUID()  == iaPattern->getDUID()) )
	{
		pdLst.append(ia);
		ia->setState(STATE_INPROCESS);
	}
    }

    if (iaLst.count() + pdLst.count() == 0) {
	// there are no IAs or PD to refresh. Just do nothing.
	return;
    }
	 
    Log(Info) << "Generating RENEW for " << iaLst.count() << " IA(s) and " << pdLst.count() << " PD(s). " << LogEnd;
    SPtr <TClntMsg> ptrRenew = new TClntMsgRenew(IfaceMgr, That, CfgMgr, AddrMgr, iaLst, pdLst);
    Transactions.append(ptrRenew);
}

void TClntTransMgr::checkDecline()
{
    //Find any tentative address and remove them from address manager
    //Group declined addresses and IAs by server and send Decline
    //Find first IA with tentative addresses
    SPtr<TAddrIA> firstIA;
    SPtr<TAddrIA> ptrIA;
    int result;
    do
    {
        firstIA=SPtr<TAddrIA>(); // NULL
        TContainer<SPtr<TAddrIA> > declineIALst;

        AddrMgr->firstIA();
        while((ptrIA=AddrMgr->getIA())&&(!firstIA))
        {
            if (ptrIA->getTentative()==TENTATIVE_YES)
                firstIA=ptrIA;
        }
        if (firstIA)
        {
            declineIALst.append(firstIA);
            while(ptrIA=AddrMgr->getIA())
            {
                if ((ptrIA->getTentative()==TENTATIVE_YES)&&
                    (*ptrIA->getDUID()==*firstIA->getDUID()))
                {
                    declineIALst.append(ptrIA);
                }

            }
            //Here should be send decline for all tentative addresses in IAs
            SPtr<TClntMsgDecline> decline = 
                new TClntMsgDecline(IfaceMgr, That, CfgMgr, AddrMgr,
                firstIA->getIface(), SPtr<TIPv6Addr>(),
                declineIALst);
            Transactions.append( (Ptr*) decline);

            // decline sent, now remove those addrs from IfaceMgr
            SPtr<TIfaceIface> ptrIface = IfaceMgr->getIfaceByID(firstIA->getIface());
	    SPtr<TDUID> duid;

            declineIALst.first();
            while (ptrIA = declineIALst.get() ) {
                SPtr<TAddrAddr> ptrAddr;
                ptrIA->firstAddr();
		Log(Info) << "Sending DECLINE for IA(IAID=" << ptrIA->getIAID() << ")" << LogEnd;

                while ( ptrAddr= ptrIA->getAddr() ) {
                    if (ptrAddr->getTentative() == TENTATIVE_YES) {
			Log(Cont) << ptrAddr->get()->getPlain() << " ";
                        // remove this address from interface
                        result = ptrIface->delAddr(ptrAddr->get(), ptrAddr->getPrefix());

                        // remove this address from addrDB
                        result = ptrIA->delAddr(ptrAddr->get());
                    }
                }
		Log(Cont) << LogEnd;
		duid = ptrIA->getDUID();
                ptrIA->setTentative();
            }

	    // create REQUEST message
	    SPtr<TClntMsgRequest> request;
	    request = new TClntMsgRequest(IfaceMgr, That, CfgMgr, AddrMgr,
					  declineIALst, duid, firstIA->getIface() );
	    Transactions.append( (Ptr*) request);

        }
    } while(firstIA);
}

void TClntTransMgr::checkRequest()
{
    /// @todo: Reimplement check request support.
    return;

    SPtr<TAddrIA> ia;
    SPtr<TClntCfgIA> cfgIA;
    SPtr<TDUID> duid = 0;
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
	if ( (cfgIA->getState()!=STATE_CONFIGURED) )
	    continue;

	// this IA is being serviced by different server
	if ( (duid) && ia->getDUID() && !((*ia->getDUID()) == (*duid)) ) {
	    continue;
	}

	// if we have found one IA, following ones must be on the same interface
	if ( ifaceID && (ia->getIface()!=ifaceID) ) {
	    continue;
	}

	// do we have enough addresses?
	// cfgIA - specified in conf file, ia - actually asigned by the server
	if ( ia->countAddr() >= cfgIA->countAddr() ) 
	    continue;
	
	Log(Info) << "IA (iaid=" << ia->getIAID() << ") is configured to get " 
		  << cfgIA->countAddr() << " addr, but server provided " << ia->countAddr() 
		  << ", REQUEST will be sent." << LogEnd;

	requestIALst.append(ia);
	ia->setState(STATE_INPROCESS);
	duid = ia->getDUID();
	ifaceID = ia->getIface();
    }

    if (requestIALst.count()) {
	// create REQUEST message
	SPtr<TClntMsgRequest> request;
	request = new TClntMsgRequest(IfaceMgr, That, CfgMgr, AddrMgr,
				      requestIALst, duid, ifaceID );
	Transactions.append( (Ptr*) request);
    } 
}

bool TClntTransMgr::isDone()
{
    return IsDone;
}

void TClntTransMgr::setContext(SPtr<TClntTransMgr> that)
{
    this->That=that;
    IfaceMgr->setContext(IfaceMgr,That,CfgMgr,AddrMgr);
}

char* TClntTransMgr::getCtrlAddr() {
	return this->ctrlAddr;
}
int  TClntTransMgr::getCtrlIface() {
	return this->ctrlIface;
}

TClntTransMgr::~TClntTransMgr() {
    Log(Debug) << "ClntTransMgr cleanup." << LogEnd;
    this->IfaceMgr->setContext(0,0,0,0);
    this->IfaceMgr  = 0;
    this->That      = 0;
    this->CfgMgr    = 0;
    this->AddrMgr   = 0;
}

void TClntTransMgr::addAdvertise(SPtr<TMsg> advertise)
{
    AdvertiseLst.append( advertise );
}

void TClntTransMgr::firstAdvertise()
{
    AdvertiseLst.first();
}

SPtr<TMsg> TClntTransMgr::getAdvertise()
{
    return AdvertiseLst.get();
}

SPtr<TOpt> TClntTransMgr::getAdvertiseDUID()
{
    if (!AdvertiseLst.count())
	return 0;
    AdvertiseLst.first();
    SPtr<TMsg> msg = AdvertiseLst.get();
    return msg->getOption(OPTION_SERVERID);
}

void TClntTransMgr::delFirstAdvertise()
{
    AdvertiseLst.first();
    AdvertiseLst.delFirst();
} 

int TClntTransMgr::getAdvertiseLstCount()
{
    return AdvertiseLst.count();
}

int TClntTransMgr::getMaxPreference()
{
    if (AdvertiseLst.count() == 0)
	return -1;
    int max = 0;
    
    SPtr<TClntMsgAdvertise> ptr;
    AdvertiseLst.first();
    while ( ptr = (Ptr*) AdvertiseLst.get() ) {
	if ( max < ptr->getPreference() )
	    max = ptr->getPreference();
    }
    return max;
}

void TClntTransMgr::printLst(List(TMsg) lst)
{
    SPtr<TMsg> x;
    SPtr<TClntMsgAdvertise> adv;
    lst.first();
    bool first = true;
    while (x = lst.get()) {
	adv = (Ptr*) x;
	Log(Debug) << "Advertise from " << adv->getInfo() << ".";
	if (first) {
	    Log(Cont) << "[using this]";
	    first = false;
	}
	Log(Cont) << LogEnd;
    }
}

void TClntTransMgr::sortAdvertiseLst()
{
    // we'll store all ADVERTISE here 
    List(TMsg) sorted;

    // sort ADVERTISE by the PREFERENCE value
    SPtr<TClntMsgAdvertise> ptr;
    while (AdvertiseLst.count()) {
	int max = getMaxPreference();
	AdvertiseLst.first();
	while ( ptr = (Ptr*) AdvertiseLst.get() ) {
	    if (ptr->getPreference() == max) 
		break;
	}
	
	// did we find it? Then append it on the end of sorted list, and delete from this new.
	if (ptr) {
	    sorted.append( (Ptr*) ptr );
	    AdvertiseLst.del();
	}
    }

    // now copy sorted list to AnswersLst
    AdvertiseLst = sorted;

}

void TClntTransMgr::printAdvertiseLst() {
    printLst(AdvertiseLst);
}
