/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DHCPServer.cpp,v 1.13 2004-09-28 21:49:32 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    string oldConf = config+"-old";
    serviceShutdown = 0;
    this->IsDone = false;
    IfaceMgr = new TSrvIfaceMgr();
    
	if ( IfaceMgr->isDone() ) {
	  std::clog << logger::logCrit << "Fatal error during IfaceMgr. Aborting." << logger::endl;
	  this->IsDone = true;
	  return;
    }
	
	IfaceMgr->dump(SRVIFACEMGR_FILE);

    TransMgr = new TSrvTransMgr(IfaceMgr, config, oldConf);
    TransMgr->setThat(TransMgr);
}

void TDHCPServer::run()
{
	boolean silent = false;
    while ( (!TransMgr->isDone()) && (!this->isDone()) ) {
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

	SmartPtr<TMsg> msg=IfaceMgr->select(timeout);
    if (!msg) 
	    continue;
	silent = false;
	int iface = msg->getIface();
    SmartPtr<TIfaceIface> ptrIface;
    ptrIface = IfaceMgr->getIfaceByID(iface);
    Log(Notice) << "Received " << msg->getName() << "(type=" << msg->getType() 
			    << ") on " << ptrIface->getName() << "/" << iface
			    << hex << ",TransID=0x" << msg->getTransID() << dec
			    << ", " << msg->countOption() << " opts:";
    SmartPtr<TOpt> ptrOpt;
    msg->firstOption();
    while (ptrOpt = msg->getOption() )
        std::clog << " " << ptrOpt->getOptType();
    std::clog << LogEnd;
    TransMgr->relayMsg(msg);
    }
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
#ifdef WIN32
    // just to break select() in WIN32 systems
	//std::clog << logger::logWarning << "Service shutdown: Sending SHUTDOWN packet on iface="
	//	<< TransMgr->getCtrlIface() << "/addr=" << TransMgr->getCtrlAddr() << logger::endl;
    //   int fd=	sock_add("", TransMgr->getCtrlIface(),"::",0,true); 
	//char buf = CONTROL_MSG;
    //   int cnt=sock_send(fd,TransMgr->getCtrlAddr(),&buf,1,DHCPCLIENT_PORT,TransMgr->getCtrlIface());
    //   sock_del(fd);
#endif
}


TDHCPServer::~TDHCPServer()
{
}

