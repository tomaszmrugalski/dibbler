/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: server-win32.cpp,v 1.15 2008-08-30 20:41:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.14  2005-08-07 18:10:59  thomson
 * 0.4.1 release.
 *
 * Revision 1.13  2005/07/27 21:57:43  thomson
 * 0.4.1 release, windows related changes.
 *
 * Revision 1.12  2005/07/26 00:03:03  thomson
 * Preparation for relase 0.4.1
 *
 * Revision 1.11  2005/07/17 21:09:54  thomson
 * Minor improvements for 0.4.1 release.
 *
 * Revision 1.10  2005/02/01 22:39:20  thomson
 * Command line service support greatly improved.
 *
 * Revision 1.9  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.8  2004/09/28 21:49:32  thomson
 * no message
 *
 * Revision 1.7  2004/09/20 20:14:54  thomson
 * *** empty log message ***
 *
 * Revision 1.6  2004/07/17 16:02:35  thomson
 * Command line minor modifications.
 *
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
	cout << " dibbler-server.exe ACTION [-d c:\\path\\to\\config\\file]" << endl
		 << " ACTION = status|start|stop|install|uninstall|run" << endl
		 << " status    - show status and exit" << endl
		 << " start     - start installed service" << endl
		 << " stop      - stop installed service" << endl
		 << " install   - install service" << endl
		 << " uninstall - uninstall service" << endl
		 << " run       - run in console" << endl << endl 
		 << " -d paramters is now optional." << endl;

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
    
    cout << DIBBLER_COPYRIGHT1 << " (SERVER, WinXP/2003/Vista/Win7 port)" << endl;
    cout << DIBBLER_COPYRIGHT2 << endl;
    cout << DIBBLER_COPYRIGHT3 << endl;
    cout << DIBBLER_COPYRIGHT4 << endl;
    cout << endl;

	// get the service object
    TSrvService * SrvService = TSrvService::getHandle();

	int i=0;
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


