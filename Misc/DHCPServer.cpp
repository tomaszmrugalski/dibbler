/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DHCPServer.cpp,v 1.8 2004-04-23 20:51:09 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    while ( (!TransMgr->isDone()) && (!this->isDone()) )
    {
	if (serviceShutdown)
	    TransMgr->shutdown();

        TransMgr->doDuties();
        unsigned int timeout = TransMgr->getTimeout();
	if (timeout == DHCPV6_INFINITY) timeout = DHCPV6_INFINITY/2;
	if (timeout == 0)        timeout = 1;
	if (serviceShutdown)     timeout = 0;
#ifdef WIN32
	// There's no easy way to break select, so just don't sleep
	// for too long.
	if (timeout>10) timeout = 10;
#endif

        std::clog << logger::logNotice << "Accepting connections. Next event in " << timeout << " second(s)." << logger::endl;
        SmartPtr<TMsg> msg=IfaceMgr->select(timeout);
        if (msg) 
        {
            std::clog << logger::logNotice << "Received msg:type=" << msg->getType() 
                << hex << " transID=" << msg->getTransID() << dec
                << ", " << msg->countOption() << " opts:";
            SmartPtr<TOpt> ptrOpt;
            msg->firstOption();
            while (ptrOpt = msg->getOption() )
                std::clog << " " << ptrOpt->getOptType() << "(" << ptrOpt->getSize() << ")";
            std::clog << logger::endl;
            TransMgr->relayMsg(msg);
        }
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

