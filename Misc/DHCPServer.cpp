/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DHCPServer.cpp,v 1.30 2008-08-17 22:41:43 thomson Exp $
 */

#include "DHCPServer.h"
#include "AddrClient.h"
#include "Logger.h"

volatile int serviceShutdown;

TDHCPServer::TDHCPServer(string config)
{
    serviceShutdown = 0;
    srand(now());
    this->IsDone = false;

    this->IfaceMgr = new TSrvIfaceMgr(SRVIFACEMGR_FILE);
    if ( this->IfaceMgr->isDone() ) {
	Log(Crit) << "Fatal error during IfaceMgr initialization." << LogEnd;
	this->IsDone = true;
	return;
    }
    this->IfaceMgr->dump();
    
    this->AddrMgr = new TSrvAddrMgr(SRVADDRMGR_FILE);
    if ( this->AddrMgr->isDone() ) {
	Log(Crit) << "Fatal error during IfaceMgr initialization." << LogEnd;
	this->IsDone = true;
	return;
    }
    this->AddrMgr->dump();

    this->CfgMgr = new TSrvCfgMgr(IfaceMgr, config, SRVCFGMGR_FILE);
    if ( this->CfgMgr->isDone() ) {
	Log(Crit) << "Fatal error during CfgMgr initialization." << LogEnd;
	this->IsDone = true;
	return;
    }
    this->CfgMgr->dump();

    this->TransMgr = new TSrvTransMgr(IfaceMgr, AddrMgr, CfgMgr, SRVTRANSMGR_FILE);
    if ( this->TransMgr->isDone() ) {
	Log(Crit) << "Fatal error during TransMgr initialization." << LogEnd;
	this->IsDone = true;
	return;
    }
    this->IfaceMgr->dump(); // dump it once more (important, if relay interfaces were added)
    this->TransMgr->dump(); 
    
    TransMgr->setContext(TransMgr);
}

void TDHCPServer::run()
{
    bool silent = false;
    while ( (!this->isDone()) && (!TransMgr->isDone()) ) {
    	if (serviceShutdown)
	    TransMgr->shutdown();
	
	TransMgr->doDuties();
	unsigned int timeout = TransMgr->getTimeout();
	if (timeout == 0)        timeout = 1;
	if (serviceShutdown)     timeout = 0;
	
	if (!silent)
	    Log(Notice) << "Accepting connections. Next event in " << timeout 
			<< " second(s)." << LogEnd;
#ifdef WIN32
	// There's no easy way to break select, so just don't sleep for too long.
	if (timeout>5) {
	    silent = true;
	    timeout = 5;
	}
#endif
	
	SmartPtr<TSrvMsg> msg=IfaceMgr->select(timeout);
	if (!msg) 
	    continue;
	silent = false;
	int iface = msg->getIface();
	SmartPtr<TIfaceIface> ptrIface;
	ptrIface = IfaceMgr->getIfaceByID(iface);
	Log(Notice) << "Received " << msg->getName() << " on " << ptrIface->getName() 
		    << "/" << iface << hex << ",TransID=0x" << msg->getTransID() 
		    << dec << ", " << msg->countOption() << " opts:";
	SmartPtr<TOpt> ptrOpt;
	msg->firstOption();
	while (ptrOpt = msg->getOption() )
	    Log(Cont) << " " << ptrOpt->getOptType();
	Log(Cont) << ", " << msg->getRelayCount() << " relay(s)." << LogEnd;
	if (CfgMgr->stateless() && ( (msg->getType()!=INFORMATION_REQUEST_MSG) &&
				     (msg->getType()!=RELAY_FORW_MSG))) {
	    Log(Warning) 
		<< "Stateful configuration related message received while running in the stateless mode. Message ignored." 
		<< LogEnd;
	    continue;
	} 
	TransMgr->relayMsg(msg);
    }
    Log(Notice) << "Bye bye." << LogEnd;
}

bool TDHCPServer::isDone() {
    return IsDone;
}

bool TDHCPServer::checkPrivileges() {
    // FIXME: check privileges
    return true;
}

void TDHCPServer::stop() {
    serviceShutdown = 1;
	Log(Warning) << "Service SHUTDOWN." << LogEnd;

}

void TDHCPServer::setWorkdir(std::string workdir) {
    if (this->CfgMgr) {
        this->CfgMgr->setWorkdir(workdir);
        this->CfgMgr->dump();
    }
}

TDHCPServer::~TDHCPServer()
{
    if (TransMgr)
	TransMgr->setContext(0);
    this->TransMgr = 0;
    this->AddrMgr  = 0;
    this->CfgMgr   = 0;
    this->IfaceMgr = 0;
}

