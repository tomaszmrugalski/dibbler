/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: DHCPClient.cpp,v 1.34 2009-03-24 23:17:18 thomson Exp $
 *                                                                           
 */

#include <iostream>
#include <stdlib.h>
#include "SmartPtr.h"
#include "DHCPClient.h"
#include "ClntTransMgr.h"

#include "IfaceMgr.h"
#include "ClntIfaceMgr.h"
#include "Logger.h"
#include "Portable.h"

volatile int serviceShutdown;
volatile int linkstateChange;

TDHCPClient::TDHCPClient(const std::string config)
  :IsDone(false)
{
    serviceShutdown = 0;
    linkstateChange = 0;
    srand(now());

    TClntIfaceMgr::instanceCreate(CLNTIFACEMGR_FILE);
    if ( ClntIfaceMgr().isDone() ) {
        Log(Crit) << "Fatal error during IfaceMgr initialization." << LogEnd;
        IsDone = true;
        return;
    }

    TClntCfgMgr::instanceCreate(config);
    if ( ClntCfgMgr().isDone() ) {
        Log(Crit) << "Fatal error during CfgMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }

	TClntAddrMgr::instanceCreate(ClntCfgMgr().getDUID(), ClntCfgMgr().useConfirm(), CLNTADDRMGR_FILE, false);
    if ( ClntAddrMgr().isDone() ) {
     	Log(Crit) << "Fatal error during AddrMgr initialization." << LogEnd;
  	    IsDone = true;
  	    return;
    }

    TClntTransMgr::instanceCreate(CLNTTRANSMGR_FILE);
    if ( TClntTransMgr::instance().isDone() ) {
        Log(Crit) << "Fatal error during TransMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }

    if (ClntCfgMgr().useConfirm())
        initLinkStateChange();
    else
	Log(Debug) << "Confirm disabled, skipping link change detection." << LogEnd;
}

/** 
 * initializes low-level link state change detection mechanism
 * 
 */
void TDHCPClient::initLinkStateChange()
{
    // Log(Debug) << "LinkState change detection not fully supported (disabled for now)." << LogEnd;
    // return; // disable this for now

    memset((void*)&this->linkstates, 0, sizeof(linkstates));

    ClntCfgMgr().firstIface();
    SPtr<TClntCfgIface> iface;
    Log(Debug) << "Initialising link-state detection for interfaces: ";
    while (iface = ClntCfgMgr().getIface()) 
    {
	linkstates.ifindex[linkstates.cnt++] = iface->getID();
	Log(Cont) << iface->getFullName() << " ";
    }
    Log(Cont) << LogEnd;
    
    link_state_change_init(&linkstates, &linkstateChange);
}

void TDHCPClient::stop() {
    serviceShutdown = 1;

#ifdef MOD_CLNT_CONFIRM
    if (ClntCfgMgr().useConfirm())
	link_state_change_cleanup();
#endif

#ifdef WIN32
    // just to break select() in WIN32 systems
    SPtr<TIfaceIface> iface = ClntIfaceMgr().getIfaceByID(ClntTransMgr().getCtrlIface());
    Log(Warning) << "Sending SHUTDOWN packet on the " << iface->getName()
        << "/" << iface->getID() << " (addr=" << ClntTransMgr().getCtrlAddr() << ")." << LogEnd;
    int fd = sock_add("", ClntTransMgr().getCtrlIface(),"::",0,true, false); 
    char buf = CONTROL_MSG;
    int cnt = sock_send(fd,ClntTransMgr().getCtrlAddr(),&buf,1,DHCPCLIENT_PORT,ClntTransMgr().getCtrlIface());
    sock_del(fd);
#endif
}

/* Function removed. Please use link_state_changed() instead */
/* void TDHCPClient::changeLinkstate(int iface_id) */

void TDHCPClient::resetLinkstate() {
    linkstates.cnt = 0;
    for (int i = 0; i<MAX_LINK_STATE_CHANGES_AT_ONCE; i++)
        linkstates.ifindex[i]=0; // ugly, but safe way of zeroing table
    linkstateChange = 0;
}


#ifdef MOD_CLNT_CONFIRM
void TDHCPClient::requestLinkstateChange(){
    linkstateChange = 1;
}
#endif

char* TDHCPClient::getCtrlIface(){
    SPtr<TIfaceIface> iface = ClntIfaceMgr().getIfaceByID(ClntTransMgr().getCtrlIface());
    return iface->getName();
;
}

void TDHCPClient::run()
{
    SPtr<TMsg> msg;
    while ( (!this->isDone()) && !ClntTransMgr().isDone() )
    {
	if (serviceShutdown)
	    ClntTransMgr().shutdown();

#ifdef MOD_CLNT_CONFIRM	
        if (linkstateChange) {
          ClntAddrMgr().setIA2Confirm(&linkstates);
          this->resetLinkstate();
        }
#endif

	ClntTransMgr().doDuties();
	
	unsigned int timeout = ClntTransMgr().getTimeout();

	if (timeout == 0)
	    timeout = 1;
	
        Log(Debug) << "Sleeping for " << timeout << " second(s)." << LogEnd;
        SPtr<TClntMsg> msg=ClntIfaceMgr().select(timeout);
	
        if (msg) {
	    int iface = msg->getIface();
	    SPtr<TIfaceIface> ptrIface;
	    ptrIface = ClntIfaceMgr().getIfaceByID(iface);
            Log(Info) << "Received " << msg->getName() << " on " << ptrIface->getName() 
		      << "/" << iface	<< hex << ",TransID=0x" << msg->getTransID() 
		      << dec << ", " << msg->countOption() << " opts:";
            SPtr<TOpt> ptrOpt;
            msg->firstOption();
            while (ptrOpt = msg->getOption() )
                Log(Cont) << " " << ptrOpt->getOptType(); 
            Log(Cont) << LogEnd;
	    
            ClntTransMgr().relayMsg(msg);
        }
    }
    Log(Notice) << "Bye bye." << LogEnd;
}

bool TDHCPClient::isDone() {
    return IsDone;
}

bool TDHCPClient::checkPrivileges() {
    /// @todo: check privileges
    return true;
}

void TDHCPClient::setWorkdir(std::string workdir) {
    ClntCfgMgr().setWorkdir(workdir);
    ClntCfgMgr().dump();
}

TDHCPClient::~TDHCPClient()
{
}
