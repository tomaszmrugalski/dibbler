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
 */

#include <iostream>
#include <string>

#include "ClntTransMgr.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "ClntCfgPD.h"
#include "ClntMsgAdvertise.h"
#include "ClntMsgRequest.h"
#include "ClntMsgRenew.h"
#include "ClntMsgRebind.h"
#include "ClntMsgReply.h"
#include "ClntMsgRelease.h"
#include "ClntMsgSolicit.h"
#include "ClntMsgInfRequest.h"
#include "ClntMsgDecline.h"
#include "ClntMsgConfirm.h"
#include "Container.h"
#include "DHCPConst.h"
#include "Logger.h"

TClntTransMgr * TClntTransMgr::Instance = 0;

void TClntTransMgr::instanceCreate(const std::string config)
{
	if (Instance) {
        Log(Crit) << "ClntTransMgr instance already exists. Internal code error!" << LogEnd;
        return;
    }
    Instance = new TClntTransMgr(config);
}

TClntTransMgr &TClntTransMgr::instance()
{
  if (!Instance)
      Log(Crit) << "Error: ClntTransMgr not initialized. Crashing in 3... 2... 1..." << LogEnd;
  return *Instance;
}

TClntTransMgr::TClntTransMgr(const std::string config)
  :IsDone(true), Shutdown(true)
{
    // should we set REUSE option during binding sockets?
#ifdef MOD_CLNT_BIND_REUSE
    BindReuse = true;
#else
    BindReuse = false;
#endif

    if (this->BindReuse)
        Log(Debug) << "Bind reuse enabled (multiple instances allowed)." << LogEnd;
    else
        Log(Debug) << "Bind reuse disabled (multiple instances not allowed)." << LogEnd;

    if (!this->openLoopbackSocket()) {
        return;
    }

    SPtr<TClntCfgIface> iface;
    ClntCfgMgr().firstIface();
    while(iface=ClntCfgMgr().getIface()) {
	if (!populateAddrMgr(iface))
	    return;

        if (!this->openSockets(iface)) {
            return;
        }
    }

    checkDB();
    ClntIfaceMgr().dump();

    Shutdown = false;
    IsDone = false;
}


bool TClntTransMgr::populateAddrMgr(SPtr<TClntCfgIface> iface)
{
    // create IAs in AddrMgr corresponding to those specified in CfgMgr.
    SPtr<TClntCfgIA> ia;
    iface->firstIA();
    while(ia=iface->getIA()) {
	if (ClntAddrMgr().getIA(ia->getIAID()))
	    continue; // there is such IA already - read from disk cache (client-AddrMgr.xml)
	SPtr<TAddrIA> addrIA = new TAddrIA(iface->getID(), TAddrIA::TYPE_IA,
                                           0, 0, ia->getT1(), ia->getT2(),
                                           ia->getIAID());
        ClntAddrMgr().addIA(addrIA);
    }

    SPtr<TClntCfgTA> ta;
    iface->firstTA();
    if (ta = iface->getTA() &&  (!ClntAddrMgr().getTA(ta->getIAID())))
    {
	// if there is such TA already, then skip adding it
	SPtr<TAddrIA> addrTA = new TAddrIA(iface->getID(), TAddrIA::TYPE_TA,
					   0, 0, DHCPV6_INFINITY, DHCPV6_INFINITY,
					   ta->getIAID());
	ClntAddrMgr().addTA(addrTA);
    }
    SPtr<TClntCfgPD> pd;
    iface->firstPD();
    while (pd = iface->getPD()) {
	if (ClntAddrMgr().getPD(pd->getIAID()))
	    continue; // there is such IA already - read from disk cache (client-AddrMgr.xml)
	SPtr<TAddrIA> addrPD = new TAddrIA(iface->getID(), TAddrIA::TYPE_PD,
					   0, 0, pd->getT1(), pd->getT2(),
					   pd->getIAID());
	ClntAddrMgr().addPD(addrPD);
    }

    return true;
}

bool TClntTransMgr::openSockets(SPtr<TClntCfgIface> iface) {

    if (iface->noConfig())       
        return true;
    
    // open socket
    SPtr<TIfaceIface> realIface = ClntIfaceMgr().getIfaceByID(iface->getID());
    if (!realIface) {
	    Log(Error) << "Interface " << iface->getFullName()
		           << " not present in system." << LogEnd;
        return false;
    }
    if (!realIface->flagUp()) {
        Log(Error) << "Interface " << realIface->getFullName()
                   << " is down. Unable to open socket." << LogEnd;
        return false;
    }
    if (!realIface->flagRunning()) {
        Log(Error) << "Interface " << realIface->getFullName()
	           << " is not running." << LogEnd;
        return false;
    }
    
    // get link-local address
    char* llAddr;
    realIface->firstLLAddress();
    llAddr=realIface->getLLAddress();
    if (!llAddr) {
        Log(Error) << "Interface " << realIface->getFullName()
	           << " does not have link-layer address. Weird." << LogEnd;
        return false;
    }

    SPtr<TIPv6Addr> addr = new TIPv6Addr(llAddr);

    // it's very important to open unicast socket first as it will be used for
    // unicast communication
    if (iface->getUnicast()) {
	Log(Notice) << "Creating socket for unicast communication on " << iface->getFullName()
		    << LogEnd;
	SPtr<TIPv6Addr> anyaddr = new TIPv6Addr("::", true); // don't bind to a specific address
	if (!realIface->addSocket(anyaddr, DHCPCLIENT_PORT, false, this->BindReuse)) {
	    Log(Crit) << "Unicast socket creation (addr=" << anyaddr->getPlain() << ") on " 
		      << iface->getFullName() << " interface failed." << LogEnd;
	    return false;
	}

    }

    Log(Notice) << "Creating socket (addr=" << *addr << ") on " 
		<< iface->getFullName() << " interface." << LogEnd;
    if (!realIface->addSocket(addr,DHCPCLIENT_PORT,true, this->BindReuse)) {
        Log(Crit) << "Socket creation (addr=" << *addr << ") on " 
		  << iface->getFullName() << " interface failed." << LogEnd;
        return false;
    }

    if (llAddr) {
	    char buf[48];
	    inet_ntop6(llAddr,buf);
	    // Log(Info) << "Socket bound to " << buf << "/port=" << DHCPCLIENT_PORT << LogEnd;
	    this->ctrlIface = realIface->getID();
	    strncpy(this->ctrlAddr,buf,48);
    } 

    return true;
}

/** this function removes expired addresses from interface and from database
 *  It must be called before AddrMgr::doDuties() is called.
 */
void TClntTransMgr::removeExpired() {

    if (ClntAddrMgr().getValidTimeout())
        return;

    SPtr<TAddrIA> ptrIA;
    SPtr<TAddrAddr> ptrAddr;
    SPtr<TIfaceIface> ptrIface;

    ClntAddrMgr().firstIA();
    while (ptrIA = ClntAddrMgr().getIA()) {
	if (ptrIA->getValidTimeout())
	    continue;

	ptrIA->firstAddr();
	while (ptrAddr = ptrIA->getAddr()) {
	    if (ptrAddr->getValidTimeout())
		continue;
	    ptrIface = ClntIfaceMgr().getIfaceByID(ptrIA->getIface());
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

    ClntAddrMgr().doDuties();
    ClntAddrMgr().firstIA();
    while ( ptrIA = ClntAddrMgr().getIA()) {
        iface = ClntCfgMgr().getIface( ptrIA->getIface() );
        if (!iface) {
            Log(Warning) << "IA (iaid=" << ptrIA->getIAID() 
                         << ") loaded from old file, but currently there is no iface with ifindex="
		            	 << ptrIA->getIface();
            // IA with non-existent iface, purge iface
            ptrIA->firstAddr();
            while (ptrAddr = ptrIA->getAddr())
                ptrIA->delAddr( ptrAddr->get() );
            ptrIA->setState(STATE_NOTCONFIGURED);
            ClntAddrMgr().delIA(ptrIA->getIAID());
        }
    }
}

bool TClntTransMgr::openLoopbackSocket() {
    SPtr<TIfaceIface> ptrIface;

    if (!this->BindReuse)
        return true;

#ifndef WIN32
    SPtr<TIfaceIface> loopback;
    ClntIfaceMgr().firstIface();
    while (ptrIface=ClntIfaceMgr().getIface()) {
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

    if (ClntCfgMgr().inactiveMode())
    {
        SPtr<TClntCfgIface> x;
        x = ClntCfgMgr().checkInactiveIfaces();
        if (x) {
	    if (!openSockets(x)) {
		Log(Crit) << "Attempt to bind activates interfaces failed."
			  << " Following operation may be unstable!" << LogEnd;
	    }
	}
    }

    removeExpired();
    ClntAddrMgr().doDuties();

    ClntAddrMgr().dump();
    ClntIfaceMgr().dump();
    ClntCfgMgr().dump();

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

#ifdef MOD_REMOTE_AUTOCONF
	checkRemoteSolicits();
#endif

    } 
    
    // This method launch the DNS update, so the checkDecline has to be done before to ensure the ip address is valid
    ClntIfaceMgr().doDuties();

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
    ClntAddrMgr().firstIA();
    while (ptrFirstIA = ClntAddrMgr().getIA()) {
        if ( (ptrFirstIA->getState() != STATE_CONFIGURED && ptrFirstIA->getState() != STATE_INPROCESS) ||
            !ptrFirstIA->countAddr() )
            ClntAddrMgr().delIA(ptrFirstIA->getIAID()); 
    }

    // normal IAs are to be released
    while (ClntAddrMgr().countIA()) {
        // clear the list
        releasedIAs.clear();

        // get first IA
        ClntAddrMgr().firstIA();
        ptrFirstIA = ClntAddrMgr().getIA();
        releasedIAs.append(ptrFirstIA);
        ClntAddrMgr().delIA( ptrFirstIA->getIAID() );

        // find similar IAs 
        while (ptrNextIA = ClntAddrMgr().getIA()) {
            if ((*(ptrFirstIA->getDUID())==*(ptrNextIA->getDUID())) &&
                (ptrFirstIA->getIface() == ptrNextIA->getIface() ) ) 
            {
                // IA serviced via this same server, add it do the list
                releasedIAs.append(ptrNextIA);

                // delete addressess from IfaceMgr
                SPtr<TAddrAddr> ptrAddr;
                SPtr<TIfaceIface> ptrIface;
                ptrIface = ClntIfaceMgr().getIfaceByID(ptrNextIA->getIface());
                if (!ptrIface) {
		            Log(Error) << "Unable to find " << ptrNextIA->getIface()
                               << " interface while releasing address." << LogEnd;
                    break;
                }
                ptrNextIA->firstAddr();

                // delete IA from AddrMgr
                ClntAddrMgr().delIA( ptrNextIA->getIAID() );
            }
        }

    	ta = 0;
        if (releasedIAs.count()) { 
	        // check if there are TA to release
	        releasedIAs.first();
	        iface = ClntCfgMgr().getIface(releasedIAs.get()->getIface());
	        if (iface && iface->countTA()) {
    		    iface->firstTA();
	    	    SPtr<TClntCfgTA> cfgTA = iface->getTA();
		        ta = ClntAddrMgr().getTA(cfgTA->getIAID());
		        cfgTA->setState(STATE_DISABLED);
	        }

    	}
	    pd = 0;
	    ClntAddrMgr().firstPD();
	    while (pd = ClntAddrMgr().getPD()) {
	        releasedPDs.append(pd);
	        SPtr<TClntCfgPD> cfgPD = ClntCfgMgr().getPD(pd->getIAID());
	        if (cfgPD)
		    cfgPD->setState(STATE_DISABLED);
	    }
	    sendRelease(releasedIAs,ta, releasedPDs);
    }

    // now check if there are any TA left
    ClntCfgMgr().firstIface();
    while (iface = ClntCfgMgr().getIface()) {
	if (iface->countTA()) {
	    iface->firstTA();
	    SPtr<TClntCfgTA> cfgTA = iface->getTA();
	    ta = ClntAddrMgr().getTA(cfgTA->getIAID());
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
    ClntAddrMgr().firstPD();
    releasedIAs.clear();
    releasedPDs.clear();
    while (pd = ClntAddrMgr().getPD()) {
	releasedPDs.append(pd);
	SPtr<TClntCfgPD> cfgPD = ClntCfgMgr().getPD(pd->getIAID());
	if (cfgPD)
	    cfgPD->setState(STATE_DISABLED);
    }
    if (releasedPDs.count())
	this->sendRelease(releasedIAs, 0, releasedPDs);
    
    //CHANGED:the following two lines are uncommented.
    doDuties(); // just to send RELEASE msg
    Transactions.clear(); // delete all transactions

    // clean up options
    ClntIfaceMgr().removeAllOpts();
}

void TClntTransMgr::relayMsg(SPtr<TClntMsg> msgAnswer)
{
    SPtr<TIfaceIface> ifaceQuestion;
    SPtr<TIfaceIface> ifaceAnswer;

    // is message valid?
    if (!msgAnswer->check())
        return ;

#ifdef MOD_REMOTE_AUTOCONF
    if (neighborInfoGet(msgAnswer->getTransID())) {
	processRemoteReply(msgAnswer);
	return;
    }
#endif 
    
    // find which message this is answer for
    bool found = false;
    SPtr<TClntMsg> msgQuestion;
    Transactions.first();
    while(msgQuestion=(Ptr*)Transactions.get()) {
        if (msgQuestion->getTransID()==msgAnswer->getTransID()) {
            found =true;
	    if (msgQuestion->getIface()!=msgAnswer->getIface()) {
		ifaceQuestion = ClntIfaceMgr().getIfaceByID(msgQuestion->getIface());
		ifaceAnswer   = ClntIfaceMgr().getIfaceByID(msgAnswer->getIface());
		Log(Warning) << "Reply for transaction 0x" << hex << msgQuestion->getTransID() << dec
			     << " sent on " << ifaceQuestion->getFullName() << " was received on interface " 
			     << ifaceAnswer->getFullName() << "." << LogEnd;
		// return; // don't return, just fix interface ID
		// useful, when sending thru eth0, but receiving via loopback
		msgAnswer->setIface(msgQuestion->getIface());
	    }

	    handleResponse(msgQuestion, msgAnswer);
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
    ClntCfgMgr().dump();
    ClntAddrMgr().dump();
}

/** 
 * handle response
 * 
 * @param question 
 * @param answer 
 * 
 * @return 
 */
bool TClntTransMgr::handleResponse(SPtr<TClntMsg> question, SPtr<TClntMsg> answer)
{
    // pre-handling hooks can be added here

    // handle actual response
    question->answer(answer);

    // post-handling hooks can be added here
    ClntIfaceMgr().notifyScripts(question, answer);

    if ( (question->getType()==REQUEST_MSG || question->getType()==SOLICIT_MSG) &&
	 (answer->getType()==REPLY_MSG) ) {
	// we got actual configuration complete here	
    }

    return true;
}

unsigned long TClntTransMgr::getTimeout()
{
    unsigned long timeout = DHCPV6_INFINITY;
    unsigned long tmp;

    if (this->IsDone) 
        return 0;

    // AddrMgr timeout
    timeout = ClntAddrMgr().getTimeout();
    tmp     = ClntAddrMgr().getTentativeTimeout();
    if (timeout > tmp)
        timeout = tmp;

    // IfaceMgr (Lifetime option) timeout
    tmp = ClntIfaceMgr().getTimeout();
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

    if (ClntCfgMgr().inactiveIfacesCnt()) {
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
void TClntTransMgr::sendRequest(TOptList requestOptions, int iface)
{
    sortAdvertiseLst();

    for (TOptList::iterator opt= requestOptions.begin();
	 opt!=requestOptions.end(); ++opt)
    {
	if (!allowOptInMsg(REQUEST_MSG, (*opt)->getOptType()))
	    opt = requestOptions.erase(opt);
    }
    SPtr<TClntMsg> ptr = new TClntMsgRequest(requestOptions, iface);
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

    SPtr<TClntCfgIface> ptrIface = ClntCfgMgr().getIface(iface);
    if (!ptrIface) {
	    Log(Error) << "Unable to find interface with ifindex=" << iface << LogEnd;
	    return;
    }
	
    Log(Notice) << "Creating RELEASE for " << IALst.count() << " IA(s), " 
		<< pdLst.count() << " PD(s), "
		<< (ta?" and TA":" (no TA)") << " on the " << ptrIface->getFullName() 
		<< " interface." << LogEnd;

    SPtr<TClntMsg> ptr = new TClntMsgRelease(iface, addr, IALst, ta, pdLst);
    Transactions.append( ptr );
}

// Send REBIND message
void TClntTransMgr::sendRebind(TOptList requestOptions, int iface) {

    for (TOptList::iterator opt= requestOptions.begin();
	 opt!=requestOptions.end(); ++opt)
    {
	if (!allowOptInMsg(REBIND_MSG, (*opt)->getOptType()))
	    opt = requestOptions.erase(opt);
    }

    SPtr<TClntMsg> ptr =  new TClntMsgRebind(requestOptions, iface);
    if (!ptr->isDone())
	Transactions.append( ptr );
}

void TClntTransMgr::sendInfRequest(TOptList requestOptions, int iface) {

    for (TOptList::iterator opt= requestOptions.begin();
	 opt!=requestOptions.end(); ++opt)
    {
	if (!allowOptInMsg(INFORMATION_REQUEST_MSG, (*opt)->getOptType()))
	    opt = requestOptions.erase(opt);
    }

    SPtr<TClntMsg> ptr = new TClntMsgInfRequest(requestOptions,iface);
    if (!ptr->isDone())
	Transactions.append( ptr );    
}

// should we send SOLICIT ?
void TClntTransMgr::checkSolicit() {

    //For every iface, every group in iface in ClntCfgMgr 
    //Enumerate IA's from this group
    SPtr<TClntCfgIface> iface;
    ClntCfgMgr().firstIface();
    while( (iface=ClntCfgMgr().getIface()) )
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
	    if (ta->getState()!=STATE_NOTCONFIGURED)
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
	    Transactions.append(new TClntMsgSolicit(iface->getID(), 0, iaLst, ta, pdLst, 
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

    ClntCfgMgr().firstIface();
    while(iface=ClntCfgMgr().getIface())
    {
	IALst.clear();
	iface->firstIA();
	while (cfgIA = iface->getIA()) {
	    ClntAddrMgr().firstIA();
	    while(ptrIA=ClntAddrMgr().getIA())
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
		new TClntMsgConfirm(iface->getID(), IALst));
	}
    }
}

void TClntTransMgr::checkInfRequest()
{
    SPtr<TClntCfgIface> iface;
    ClntCfgMgr().firstIface();
    while( (iface=ClntCfgMgr().getIface()) )
    {
        if (iface->noConfig())
            continue;
	SPtr<TClntIfaceIface> ifaceIface = (Ptr*)ClntIfaceMgr().getIfaceByID(iface->getID());
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
            Transactions.append(new TClntMsgInfRequest(iface));
	}
    }
}

void TClntTransMgr::checkRenew()
{
    // are there any IAs which require RENEW?
    if (ClntAddrMgr().getT1Timeout() > 0 ) 
	return;

    // TENTATIVE_YES, there are. Find them!
    
    // Find all IAs
    List(TAddrIA) iaLst;
    SPtr<TAddrIA> ia;
    SPtr<TAddrIA> iaPattern;
    ClntAddrMgr().firstIA();

    // Need to be fixed:?? how to deal with mutiple network interfaces.
    while (ia = ClntAddrMgr().getIA() ) 
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
    ClntAddrMgr().firstPD();
    while (ia = ClntAddrMgr().getPD()) {
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
    SPtr <TClntMsg> ptrRenew = new TClntMsgRenew(iaLst, pdLst);
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

        ClntAddrMgr().firstIA();
        while((ptrIA=ClntAddrMgr().getIA())&&(!firstIA))
        {
            if (ptrIA->getTentative()==TENTATIVE_YES)
                firstIA=ptrIA;
        }
        if (firstIA)
        {
            declineIALst.append(firstIA);
            while(ptrIA=ClntAddrMgr().getIA())
            {
                if ((ptrIA->getTentative()==TENTATIVE_YES)&&
                    (*ptrIA->getDUID()==*firstIA->getDUID()))
                {
                    declineIALst.append(ptrIA);
                }

            }
            //Here should be send decline for all tentative addresses in IAs
            SPtr<TClntMsgDecline> decline = 
                new TClntMsgDecline(firstIA->getIface(), 0, declineIALst);
            Transactions.append( (Ptr*) decline);

            // decline sent, now remove those addrs from IfaceMgr
            SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(firstIA->getIface());
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
	    request = new TClntMsgRequest(declineIALst, duid, firstIA->getIface() );
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
    ClntAddrMgr().firstIA();
    while( ia=ClntAddrMgr().getIA() ) {

	cfgIA = ClntCfgMgr().getIA(ia->getIAID());
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
	request = new TClntMsgRequest( requestIALst, duid, ifaceID );
	Transactions.append( (Ptr*) request);
    } 
}

bool TClntTransMgr::isDone()
{
    return IsDone;
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

#ifdef MOD_REMOTE_AUTOCONF
SPtr<TClntTransMgr::TNeighborInfo> TClntTransMgr::neighborInfoGet(SPtr<TIPv6Addr> addr) {
    for (TNeighborInfoLst::iterator it=Neighbors.begin(); it!=Neighbors.end(); ++it) {
	if ( (*(*it)->srvAddr) == *addr)
	    return *it;
    }
    return 0;
}

SPtr<TClntTransMgr::TNeighborInfo> TClntTransMgr::neighborInfoGet(int transid) {
  for (TNeighborInfoLst::iterator it=Neighbors.begin(); it!=Neighbors.end(); ++it) {
      if ( (*it)->transid == transid)
	  return *it;
  }

  return 0;
}

bool TClntTransMgr::updateNeighbors(int ifindex, SPtr<TOptAddrLst> neighbors) {
  neighbors->firstAddr();

  SPtr<TIPv6Addr> addr;
  while (addr=neighbors->getAddr()) {
      SPtr<TNeighborInfo> info;
      info = neighborInfoGet(addr);
      if (info)
	  continue; // this neighbor is already known
      neighborAdd(ifindex, addr);
  }

  return true;
}

SPtr<TClntTransMgr::TNeighborInfo> TClntTransMgr::neighborAdd(int ifindex, SPtr<TIPv6Addr> addr) {
    SPtr<TNeighborInfo> info = new TNeighborInfo(addr);
    info->srvAddr = addr;
    info->ifindex = ifindex;

    Log(Debug) << "New information about neighbor " << addr->getPlain() << " added." << LogEnd;
    Neighbors.push_back(info);

    // it's too early to send remote solicit.
    // we will send it once global address is received
    // sendRemoteSolicit(info); 

    return info;
}

bool TClntTransMgr::checkRemoteSolicits() {
    bool status = true;

    if (!ClntAddrMgr().getPreferredAddr()) {
	// There are no preferred (non-tentative) addresses. Skipping remote solicit.
	return false;
    }

    // There is preferred address: " << ClntAddrMgr().getPreferredAddr()->getPlain()
    
    for (TNeighborInfoLst::iterator it=Neighbors.begin(); it!=Neighbors.end(); ++it) {
	if ( (*it)->state != TNeighborInfo::NeighborInfoState_Added)
	    continue; 
	
	status = sendRemoteSolicit(*it) && status;
	(*it)->state = TNeighborInfo::NeighborInfoState_Sent;
    }

    return status;
}

bool TClntTransMgr::sendRemoteSolicit(SPtr<TNeighborInfo> neighbor) {
    Log(Debug) << "Sending remote Solicit to " << neighbor->srvAddr->getPlain() << LogEnd;

    SPtr<TClntCfgIface> iface;
    ClntCfgMgr().firstIface();
    while( (iface=ClntCfgMgr().getIface()) )
    {
	if (iface->getID() == neighbor->ifindex)
	    break;
    }
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << neighbor->ifindex 
		   << ". Remote solicit failed." << LogEnd;
	return false;
    }

    List(TClntCfgIA) iaLst; // list of IA requiring configuration
    SPtr<TClntCfgIA> cfgIA;
    iface->firstIA();
    while (cfgIA=iface->getIA()) {
	iaLst.append(cfgIA);
    }

    // ignore TA & PD
    Log(Info) << "Creating remote SOLICIT to be transmitted to " << neighbor->srvAddr->getPlain() << LogEnd;
    SPtr<TClntCfgTA> ta; // use empty pointer
    List(TClntCfgPD) pdLst; // use empty list

    SPtr<TClntMsg> solicit = new TClntMsgSolicit(neighbor->ifindex, neighbor->srvAddr,
						 iaLst, ta, pdLst,
						 true /*rapid-commit */, 
						 true /* remote autoconf*/);
    neighbor->transid = solicit->getTransID();
    Transactions.append(solicit);

    return true;
}

bool TClntTransMgr::processRemoteReply(SPtr<TClntMsg> reply) {

    Log(Debug) << "Processing remote REPLY to remote SOLICIT." << LogEnd;
    int xid = reply->getTransID();
    SPtr<TNeighborInfo> neigh = neighborInfoGet(xid);
    if (!neigh) {
	Log(Error) << "Failed to match transmitted remote SOLICIT. Seems like bogus remote REPLY." << LogEnd;
	return false;
    }

    SPtr<TClntMsgReply> rpl = (Ptr*) reply;
    neigh->reply = reply;
    neigh->rcvdAddr = rpl->getFirstAddr();
    neigh->state = TNeighborInfo::NeighborInfoState_Received;

    Transactions.first();
    SPtr<TClntMsg> sol;
    while (sol = Transactions.get() ) {
	if (sol->getTransID() == neigh->transid)
	    sol->isDone(true);
    }

    return ClntIfaceMgr().notifyRemoteScripts(neigh->rcvdAddr, neigh->srvAddr, reply->getIface() );
}

#endif
