/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *          win2k version by <sob@hisoftware.cz>
 *
 * based on server-winxp.cpp,v 1.10 2005/02/01 22:39:20 thomson Exp $
 *
 * $Id: server-winnt2k.cpp,v 1.1 2005-07-23 14:33:22 thomson Exp $
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
extern TDHCPServer * svptr;

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
	  svptr->stop();
      return TRUE;
  }
    case CTRL_BREAK_EVENT: 
      return FALSE; 
  }
  return TRUE;
}

int main(int argc, char* argv[]) {
    // get the service object
    TSrvService * SrvService = TSrvService::getHandle();

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
    
    cout << DIBBLER_COPYRIGHT1 << " (SERVER)" << endl;
    cout << DIBBLER_COPYRIGHT2 << endl;
    cout << DIBBLER_COPYRIGHT3 << endl;
    cout << DIBBLER_COPYRIGHT4 << endl;
    cout << endl;
    
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
 */
