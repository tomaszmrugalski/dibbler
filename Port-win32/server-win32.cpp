/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: server-win32.cpp,v 1.6 2004-07-17 16:02:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
extern TDHCPServer * ptr;

void usage() {
	cout << "Usage:" << endl;
	cout << " dibbler-server-winxp.exe ACTION -d dirname " << endl
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
	  ptr->stop();
      return TRUE;
  }
    case CTRL_BREAK_EVENT: 
      return FALSE; 
  }
  return TRUE;
}

int main(int argc, char* argv[]) {
    // Create the service object
    TSrvService SrvService;
	int i=0;
    WSADATA wsaData;
	if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData )) {
			clog<<"Unable to load WinSock 2.2"<<endl;
			return 0;
	}

	// find ipv6.exe (or netsh.exe in future implementations)
	if (!lowlevelInit()) {
		clog << "lowlevelInit() failed. Startup aborted.\n";
		return -1;		
	}

	EServiceState status = SrvService.ParseStandardArgs(argc, argv);
	SrvService.setState(status);

	clog << DIBBLER_COPYRIGHT1 << " (SERVER)" << endl;
	clog << DIBBLER_COPYRIGHT2 << endl;
	clog << DIBBLER_COPYRIGHT3 << endl;
	clog << DIBBLER_COPYRIGHT4 << endl;
	clog << endl;

	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );

	switch (status) {
	case STATUS: {
		clog << "Service: ";
		if (SrvService.IsInstalled()) 
			clog << "INSTALLED " << endl;
		else
			clog << "NOT INSTALLED" << endl;
 		break;
	};
	case START: {
		SrvService.StartService();
	break;
	}
	case STOP: {
		clog << "FIXME: STOP function is not implemented yet." << endl;
		break;
	}
	case INSTALL: {
		SrvService.Install();
		break;
	}
	case UNINSTALL: {
		SrvService.Uninstall();
		break;
	} 
	case RUN: {
		SrvService.Run();
		break;
	}
  	case INVALID: {
		clog << "Invalid usage." << endl;
    }				  
	case HELP: 
	default: {
		usage();
	}	
	}

    return 0;
}