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

    TSrvIfaceMgr::instanceCreate(SRVIFACEMGR_FILE);
    if ( SrvIfaceMgr().isDone() ) {
	      Log(Crit) << "Fatal error during IfaceMgr initialization." << LogEnd;
	      this->IsDone = true;
	      return;
    }
    SrvIfaceMgr().dump();
    
    TSrvCfgMgr::instanceCreate(config, SRVCFGMGR_FILE);
    if ( SrvCfgMgr().isDone() ) {
        Log(Crit) << "Fatal error during CfgMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }
    SrvCfgMgr().dump();

    TSrvAddrMgr::instanceCreate(SRVADDRMGR_FILE, true /*always load DB*/ );
    if ( SrvAddrMgr().isDone() ) {
        Log(Crit) << "Fatal error during AddrMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }
    SrvAddrMgr().dump();

    TSrvTransMgr::instanceCreate(SRVTRANSMGR_FILE);
    if ( SrvTransMgr().isDone() ) {
        Log(Crit) << "Fatal error during TransMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }

    SrvCfgMgr().setCounters();
    SrvCfgMgr().dump();
    SrvIfaceMgr().dump(); // dump it once more (important, if relay interfaces were added)
    SrvTransMgr().dump();
}

void TDHCPServer::run()
{	
    bool silent = false;
    while ( (!isDone()) && (!SrvTransMgr().isDone()) ) {
    	if (serviceShutdown)
	    SrvTransMgr().shutdown();
	
	SrvTransMgr().doDuties();
	unsigned int timeout = SrvTransMgr().getTimeout();
	if (timeout == 0)        timeout = 1;
	if (serviceShutdown)     timeout = 0;
	
	if (!silent)
	    Log(Notice) << "Accepting connections. Next event in " << timeout 
			<< " second(s)." << LogEnd;
#ifdef WIN32
	// There's no easy way to break select under Windows, so just don't sleep for too long.
	if (timeout>5) {
	    silent = true;
	    timeout = 5;
	}
#endif
	
	SPtr<TSrvMsg> msg=SrvIfaceMgr().select(timeout);
	if (!msg) 
	    continue;
	silent = false;
	int iface = msg->getIface();
	SPtr<TIfaceIface> ptrIface;
	ptrIface = SrvIfaceMgr().getIfaceByID(iface);
	Log(Notice) << "Received " << msg->getName() << " on " << ptrIface->getName() 
		    << "/" << iface << hex << ",TransID=0x" << msg->getTransID() 
		    << dec << ", " << msg->countOption() << " opts:";
	SPtr<TOpt> ptrOpt;
	msg->firstOption();
	while (ptrOpt = msg->getOption() )
	    Log(Cont) << " " << ptrOpt->getOptType();
	Log(Cont) << ", " << msg->getRelayCount() << " relay(s)." << LogEnd;
	if (SrvCfgMgr().stateless() && ( (msg->getType()!=INFORMATION_REQUEST_MSG) &&
					 (msg->getType()!=RELAY_FORW_MSG))) {
	    Log(Warning) 
		<< "Stateful configuration related message received while running in the stateless mode. Message ignored." 
		<< LogEnd;
	    continue;
	} 
	SrvTransMgr().relayMsg(msg);
    }
    Log(Notice) << "Bye bye." << LogEnd;
}

bool TDHCPServer::isDone() {
    return IsDone;
}

bool TDHCPServer::checkPrivileges() {
    /// @todo: check privileges
    return true;
}

void TDHCPServer::stop() {
    serviceShutdown = 1;
	Log(Warning) << "Service SHUTDOWN." << LogEnd;

}

void TDHCPServer::setWorkdir(std::string workdir) {
    SrvCfgMgr().setWorkdir(workdir);
    SrvCfgMgr().dump();
}

TDHCPServer::~TDHCPServer()
{
}

