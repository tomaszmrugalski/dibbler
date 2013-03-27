/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "DHCPServer.h"
#include "AddrClient.h"
#include "Logger.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"

using namespace std;

volatile int serviceShutdown;

TDHCPServer::TDHCPServer(const std::string& config)
{
    serviceShutdown = 0;
    srand(now());
    IsDone = false;

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

    TSrvTransMgr::instanceCreate(SRVTRANSMGR_FILE, DHCPSERVER_PORT);
    if ( SrvTransMgr().isDone() ) {
        Log(Crit) << "Fatal error during TransMgr initialization." << LogEnd;
        this->IsDone = true;
        return;
    }

    SrvCfgMgr().removeReservedFromCache();
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
	SPtr<TIfaceIface>  physicalIface = SrvIfaceMgr().getIfaceByID(msg->getPhysicalIface());
	SPtr<TSrvCfgIface> logicalIface = SrvCfgMgr().getIfaceByID(msg->getIface());
        if (!physicalIface) {
            Log(Error) << "Received data over unknown physical interface: ifindex="
		       << msg->getPhysicalIface() << LogEnd;
            continue;
        }
	if (!logicalIface) {
	    Log(Error) << "Received data over unknown logical interface: ifindex="
		       << msg->getIface() << LogEnd;
	    continue;
	}
	Log(Notice) << "Received " << msg->getName() << " on " << physicalIface->getFullName()
		    << hex << ", trans-id=0x" << msg->getTransID() << dec
		    << ", " << msg->countOption() << " opts:";
	SPtr<TOpt> ptrOpt;
	msg->firstOption();
	while (ptrOpt = msg->getOption() )
	    Log(Cont) << " " << ptrOpt->getOptType();
	if (msg->RelayInfo_.size()) {
	    Log(Cont) << " (" << logicalIface->getFullName() << ", "
		      << msg->RelayInfo_.size() << " relay(s)." << LogEnd;
	} else {
	    Log(Cont) << " (non-relayed)" << LogEnd;
	}

	if (SrvCfgMgr().stateless() && ( (msg->getType()!=INFORMATION_REQUEST_MSG) &&
					 (msg->getType()!=RELAY_FORW_MSG))) {
	    Log(Warning) 
		<< "Stateful configuration message received while running in "
		<< "the stateless mode. Message ignored." << LogEnd;
	    continue;
	} 
	SrvTransMgr().relayMsg(msg);
    }
    SrvIfaceMgr().closeSockets();
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

