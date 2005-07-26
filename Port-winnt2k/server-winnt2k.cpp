/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *          win2k version by <sob@hisoftware.cz>
 *
 * based on server-winxp.cpp,v 1.10 2005/02/01 22:39:20 thomson Exp $
 *
 * $Id: server-winnt2k.cpp,v 1.3 2005-07-26 00:03:03 thomson Exp $
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <string>
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#include "Portable.h"
#include "DHCPServer.h"

#include "WinService.h"
#include "SrvService.h"
#include "Logger.h"

extern "C" int lowlevelInit();
extern TDHCPServer * srvPtr;

void usage() {
	cout << "Usage:" << endl;
	cout << " server-winnt2k.exe ACTION -d dirname " << endl
		 << " ACTION = status|start|stop|install|uninstall|run" << endl
		 << " status    - show status and exit" << endl
		 << " start     - start installed service" << endl
		 << " stop      - stop installed service" << endl
		 << " install   - install service" << endl
		 << " uninstall - uninstall service" << endl
		 << " run       - run in console" << endl;

}

/* 
 * Handle the CTRL-C, CTRL-BREAK signal. 
 */
BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
  switch( fdwCtrlType ) 
  { 
  case CTRL_C_EVENT: {
	  srvPtr->stop();
      return TRUE;
  }
    case CTRL_BREAK_EVENT: 
      return FALSE; 
  }
  return TRUE;
}

int main(int argc, char* argv[]) {
    cout << DIBBLER_COPYRIGHT1 << " (SERVER, WinNT/2000 port)" << endl;
    cout << DIBBLER_COPYRIGHT2 << endl;
    cout << DIBBLER_COPYRIGHT3 << endl;
    cout << DIBBLER_COPYRIGHT4 << endl;
    cout << endl;

    // get the service object
    TSrvService * SrvService = TSrvService::getHandle();

    // load winsock library if it is not already loaded
    WSADATA wsaData;
    if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData )) {
        cout<<"Unable to load WinSock 2.2"<<endl;
	    return 0;
    }
    
    // find ipv6.exe (or netsh.exe in future implementations)
    if (!lowlevelInit()) {
        cout << "lowlevelInit() failed. Startup aborted.\n";
        return -1;		
    }
    
    EServiceState status = SrvService->ParseStandardArgs(argc, argv);
    SrvService->setState(status);

    // is this proper port?
	if (!SrvService->verifyPort()) {
       Log(Crit) << "Operating system version is not supported by this Dibbler port." << LogEnd;
       return -1;
    }
    
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
    
    switch (status) {
    case STATUS: {
	    SrvService->showStatus();
    	break;
    };
    case START: {
	    SrvService->StartService();
	    break;
    }
    case STOP: {
        SrvService->StopService();
	    break;
    }
    case INSTALL: {
	    SrvService->Install();
	    break;
    }
    case UNINSTALL: {
	    SrvService->Uninstall();
	    break;
    } 
    case RUN: {
	    SrvService->Run();
	    break;
    }
    case SERVICE: {
	    SrvService->RunService();
	    break;
    }
    case INVALID: {
    	cout << "Invalid usage." << endl;
    }				  
    case HELP: 
    default: {
	    usage();
    }	
    }
    
    return 0;
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/07/24 16:00:03  thomson
 * Port WinNT/2000 related changes.
 *
 * Revision 1.1  2005/07/23 14:33:22  thomson
 * Port for win2k/NT added.
 *
 */
