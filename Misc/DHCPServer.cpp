/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */
      
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
    }

    TransMgr = new TSrvTransMgr(IfaceMgr, config, oldConf);
    TransMgr->setThat(TransMgr);
}

void TDHCPServer::run()
// 1. W zale¿no¶ci czy:  
// a. wyst±pi³ timeoutu - wywo³anie doDuties
// b. przyszla wiadomo¶æ - wywo³anie relayMessage
// 
// powtarzanie: (do nadrobienia czasu)
// 2. wywolanie getTimeout
// 3. wywolanie doDuties 
// 
// 4. Za¶niêcie w oczekiwaniu na przyj¶cie wiadomo¶ci lub up³yniêcie
// tiemoutu tj. wywo³anie SELECT
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

        std::clog << logger::logNotice << "Sleeping for " << timeout << " seconds." << logger::endl;
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
    //clog<<logger::logInfo<<"Service shutdown."<<logger::endl;
    //int fd=sock_add("",1,"::1",DHCPCLIENT_PORT,true);
    //char buf = 0;
    //sock_send(fd,"::1",buf,1,DHCPCLIENT_PORT,1);
    //sock_del(fd);
#endif
}


TDHCPServer::~TDHCPServer()
{
}

