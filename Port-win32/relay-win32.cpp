/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: relay-win32.cpp,v 1.2 2005-02-01 01:09:34 thomson Exp $
 *
 * Released under GNU GPL v2 licence
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/24 00:42:37  thomson
 * no message
 *
 *
 */

#include <string>
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#include "Portable.h"
#include "DHCPRelay.h"

#include "WinService.h"
#include "RelService.h"
#include "Logger.h"

extern "C" int lowlevelInit();
extern TDHCPRelay * ptr;

void usage() {
	cout << "Usage:" << endl;
	cout << " dibbler-relay.exe ACTION -d dirname " << endl
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
    TRelService RelService;
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
    
    EServiceState status = RelService.ParseStandardArgs(argc, argv);
    RelService.setState(status);
    
    cout << DIBBLER_COPYRIGHT1 << " (RELAY)" << endl;
    cout << DIBBLER_COPYRIGHT2 << endl;
    cout << DIBBLER_COPYRIGHT3 << endl;
    cout << DIBBLER_COPYRIGHT4 << endl;
    cout << endl;
    
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
    
    switch (status) {
    case STATUS: {
        RelService.showStatus();
#if 0
        cout << "Service: ";
	if (RelService.IsInstalled()) 
	    cout << "INSTALLED " << endl;
	else
	    cout << "NOT INSTALLED" << endl;
#endif
	break;
    };
    case START: {
	    RelService.StartService();
	    break;
    }
    case STOP: {
	    RelService.StopService();
    	break;
    }
    case INSTALL: {
        RelService.Install();
	    break;
    }
    case UNINSTALL: {
	    RelService.Uninstall();
	    break;
    } 
    case RUN: {
	    RelService.Run();
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


