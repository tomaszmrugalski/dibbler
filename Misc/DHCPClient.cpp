/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: DHCPClient.cpp,v 1.33 2008-11-13 22:18:17 thomson Exp $
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

TDHCPClient::TDHCPClient(string config)
{
    serviceShutdown = 0;
    linkstateChange = 0;
    srand(now());
    this->IsDone = false;

    IfaceMgr = new TClntIfaceMgr(CLNTIFACEMGR_FILE);
    if ( this->IfaceMgr->isDone() ) {
 	  Log(Crit) << "Fatal error during IfaceMgr initialization." << LogEnd;
	  this->IsDone = true;
	  return;
    }

    this->CfgMgr = new TClntCfgMgr(IfaceMgr, config);
    if ( this->CfgMgr->isDone() ) {
	Log(Crit) << "Fatal error during CfgMgr initialization." << LogEnd;
	this->IsDone = true;
	return;
    }

    this->AddrMgr = new TClntAddrMgr(CfgMgr, CLNTADDRMGR_FILE, false);
    if ( this->AddrMgr->isDone() ) {
 	  Log(Crit) << "Fatal error during AddrMgr initialization." << LogEnd;
	  this->IsDone = true;
	  return;
    }

    this->TransMgr = new TClntTransMgr(IfaceMgr, AddrMgr, CfgMgr, CLNTTRANSMGR_FILE);
    if ( this->TransMgr->isDone() ) {
	Log(Crit) << "Fatal error during TransMgr initialization." << LogEnd;
	this->IsDone = true;
	return;
    }

    if (CfgMgr->useConfirm())
	initLinkStateChange();

    TransMgr->setContext(TransMgr);
}

/** 
 * initializes low-level link state change detection mechanism
 * 
 */
void TDHCPClient::initLinkStateChange()
{
    Log(Debug) << "LinkState change detection not fully supported (disabled for now)." << LogEnd;
    return; // disable this for now

    memset((void*)&this->linkstates, 0, sizeof(linkstates));

    CfgMgr->firstIface();
    SPtr<TClntCfgIface> iface;
    Log(Debug) << "Initialising link-state detection for interfaces: ";
    while (iface = CfgMgr->getIface()) 
    {
	linkstates.ifindex[linkstates.cnt++] = iface->getID();
	Log(Cont) << iface->getID() << " ";
    }
    Log(Cont) << LogEnd;
    
    link_state_change_init(&linkstates, &linkstateChange);
}

void TDHCPClient::stop() {
    serviceShutdown = 1;

    if (CfgMgr->useConfirm())
	link_state_change_cleanup();

#ifdef WIN32
    // just to break select() in WIN32 systems
    SmartPtr<TIfaceIface> iface = IfaceMgr->getIfaceByID(TransMgr->getCtrlIface());
    Log(Warning) << "Sending SHUTDOWN packet on the " << iface->getName()
        << "/" << iface->getID() << " (addr=" << TransMgr->getCtrlAddr() << ")." << LogEnd;
    int fd = sock_add("", TransMgr->getCtrlIface(),"::",0,true, false); 
    char buf = CONTROL_MSG;
    int cnt=sock_send(fd,TransMgr->getCtrlAddr(),&buf,1,DHCPCLIENT_PORT,TransMgr->getCtrlIface());
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

char* TDHCPClient::getCtrlIface(){
    SmartPtr<TIfaceIface> iface = IfaceMgr->getIfaceByID(TransMgr->getCtrlIface());
    return iface->getName();
;
}

void TDHCPClient::run()
{
    SmartPtr<TMsg> msg;
    while ( (!this->isDone()) && !TransMgr->isDone() )
    {
	if (serviceShutdown)
	    TransMgr->shutdown();
	
        if (linkstateChange) {
	    AddrMgr->setIA2Confirm(&linkstates);
	    this->resetLinkstate();
        }

	TransMgr->doDuties();
	
	unsigned int timeout = TransMgr->getTimeout();

	if (timeout == 0)
	    timeout = 1;
	
        Log(Debug) << "Sleeping for " << timeout << " second(s)." << LogEnd;
        SmartPtr<TClntMsg> msg=IfaceMgr->select(timeout);
	
        if (msg) {
	    int iface = msg->getIface();
	    SmartPtr<TIfaceIface> ptrIface;
	    ptrIface = IfaceMgr->getIfaceByID(iface);
            Log(Info) << "Received " << msg->getName() << " on " << ptrIface->getName() 
		      << "/" << iface	<< hex << ",TransID=0x" << msg->getTransID() 
		      << dec << ", " << msg->countOption() << " opts:";
            SmartPtr<TOpt> ptrOpt;
            msg->firstOption();
            while (ptrOpt = msg->getOption() )
                Log(Cont) << " " << ptrOpt->getOptType(); 
            Log(Cont) << LogEnd;
	    
            TransMgr->relayMsg(msg);
        }
    }
    Log(Notice) << "Bye bye." << LogEnd;
}

bool TDHCPClient::isDone() {
    return IsDone;
}

bool TDHCPClient::checkPrivileges() {
    // FIXME: check privileges
    return true;
}

void TDHCPClient::setWorkdir(std::string workdir) {
    if (this->CfgMgr) {
        this->CfgMgr->setWorkdir(workdir);
        this->CfgMgr->dump();
    }
}

SmartPtr<TClntAddrMgr> TDHCPClient::getAddrMgr() {
    return this->AddrMgr;
}

SmartPtr<TClntCfgMgr> TDHCPClient::getCfgMgr() {
    return this->CfgMgr;
}

TDHCPClient::~TDHCPClient()
{
    if (TransMgr)
	TransMgr->setContext(0);
    this->TransMgr = 0;
    this->AddrMgr  = 0;
    this->CfgMgr   = 0;
    this->IfaceMgr = 0;
}
