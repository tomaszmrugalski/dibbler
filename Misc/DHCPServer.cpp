/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DHCPServer.cpp,v 1.23 2005-02-07 20:51:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.22  2005/02/01 00:57:36  thomson
 * no message
 *
 * Revision 1.21  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 * Revision 1.20  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.19  2005/01/03 21:54:49  thomson
 * Mgr dumps added.
 *
 * Revision 1.18  2004/12/07 00:45:09  thomson
 * Manager creation unified and cleaned up.
 *
 * Revision 1.17  2004/12/02 00:51:04  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.16  2004/11/01 23:31:24  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.15  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.14  2004/10/02 13:11:24  thomson
 * Boolean options in config file now can be specified with YES/NO/TRUE/FALSE.
 * Unicast communication now can be enable on client side (disabled by default).
 *
 * Revision 1.13  2004/09/28 21:49:32  thomson
 * no message
 *
 * Revision 1.12  2004/07/05 00:12:29  thomson
 * Lots of minor changes.
 *
 * Revision 1.11  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 * Revision 1.10  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 * Revision 1.9  2004/05/24 21:16:37  thomson
 * Various fixes.
 *
 * Revision 1.8  2004/04/23 20:51:09  thomson
 * Changes informational message. (closes bug #21)
 *
 * Revision 1.7  2004/03/28 19:57:59  thomson
 * no message
 *
 */
      
#include "DHCPServer.h"
#include "AddrClient.h"
#include "Logger.h"

#ifdef  WIN32
#include <crtdbg.h>
#endif

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
    
    TransMgr->setThat(TransMgr);
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
    if (this->CfgMgr)
        this->CfgMgr->setWorkdir(workdir);
    this->CfgMgr->dump();
}

TDHCPServer::~TDHCPServer()
{
}

