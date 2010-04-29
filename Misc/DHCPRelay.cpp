/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */
      
#include "DHCPRelay.h"
#include "Logger.h"
#include "Portable.h"
#include "RelIfaceMgr.h"
#include "RelCfgMgr.h"
#include "RelTransMgr.h"
#include "RelMsg.h"

volatile int serviceShutdown;

TDHCPRelay::TDHCPRelay(string config)
{
    serviceShutdown = 0;
    srand(now());
    IsDone = false;

    TRelIfaceMgr::instanceCreate(RELIFACEMGR_FILE);
    if ( RelIfaceMgr().isDone() ) {
        Log(Crit) << "Fatal error during IfaceMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }
    RelIfaceMgr().dump();

    TRelCfgMgr::instanceCreate(config, RELCFGMGR_FILE);
    if ( RelCfgMgr().isDone() ) {
        Log(Crit) << "Fatal error during CfgMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }
    RelCfgMgr().dump();

    TRelTransMgr::instanceCreate(RELTRANSMGR_FILE);
    if ( RelTransMgr().isDone() ) {
        Log(Crit) << "Fatal error during TransMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }
    RelIfaceMgr().dump();
    RelTransMgr().dump();
}

void TDHCPRelay::run()
{
    bool silent = false;
    while ( (!isDone()) && (!RelTransMgr().isDone()) ) {
    	if (serviceShutdown)
	    RelTransMgr().shutdown();
	
	RelTransMgr().doDuties();
	unsigned int timeout = DHCPV6_INFINITY/2;
	if (serviceShutdown)     timeout = 0;
	
	if (!silent)
	    Log(Debug) << "Accepting messages." << LogEnd;

#ifdef WIN32
	// There's no easy way to break select, so just don't sleep for too long.
	if (timeout>5) {
	    silent = true;
	    timeout = 5;
	}
#endif
	
	SPtr<TRelMsg> msg = RelIfaceMgr().select(timeout);
	if (!msg) 
	    continue;
	silent = false;
	int iface = msg->getIface();
	SPtr<TIfaceIface> ptrIface;
	ptrIface = RelIfaceMgr().getIfaceByID(iface);
	Log(Notice) << "Received " << msg->getName() << " on " << ptrIface->getName() 
		    << "/" << iface;
	if (msg->getType()!=RELAY_FORW_MSG && msg->getType()!=RELAY_REPL_MSG)
	    Log(Cont) << hex << ",TransID=0x" << msg->getTransID() << dec;
	Log(Cont) << ", " << msg->countOption() << " opts:";
	SPtr<TOpt> ptrOpt;
	msg->firstOption();
	while ( ptrOpt = msg->getOption() ) {
	    Log(Cont) << " " << ptrOpt->getOptType(); 
            // uncomment this to get detailed info about option lengths Log(Cont) << "/" << ptrOpt->getSize();
	}
	// Log(Cont) << ", " << msg->getRelayCount() << " relay(s).";
	Log(Cont) << LogEnd;
	RelTransMgr().relayMsg(msg);
    }
    Log(Notice) << "Bye bye." << LogEnd;
}

bool TDHCPRelay::isDone() {
    return this->IsDone;
}

bool TDHCPRelay::checkPrivileges() {
    /// @todo: check privileges
    return true;
}

void TDHCPRelay::stop() {
    serviceShutdown = 1;
	Log(Crit) << "Service SHUTDOWN." << LogEnd;

}

void TDHCPRelay::setWorkdir(std::string workdir) {
    RelCfgMgr().setWorkdir(workdir);
    RelCfgMgr().dump();
}

TDHCPRelay::~TDHCPRelay() {
}

