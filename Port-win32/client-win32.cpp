/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: client-win32.cpp,v 1.10 2004-04-15 23:53:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2004/04/15 23:24:44  thomson
 * Pathname installation fixed, run-time error checks disabled, winXP code cleanup.
 *
 * Revision 1.8  2004/03/29 22:06:49  thomson
 * 0.1.1 version
 *
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <string>
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#include "WinService.h"
#include "ClntService.h"
#include "Portable.h"
#include "DHCPClient.h"
#include "logger.h"
#include <iostream>
#include <crtdbg.h>

extern "C" int lowlevelInit();

using namespace std;

void usage() {
	cout << "Usage:" << endl;
	cout << " dibbler-client-winxp.exe ACTION -d dirname" << endl
		 << " ACTION = status|start|stop|install|uninstall|run" << endl
		 << " status    - show status and exit" << endl
		 << " start     - start installed service" << endl
		 << " stop      - stop installed service" << endl
		 << " install   - install service" << endl
		 << " uninstall - uninstall service" << endl
		 << " run       - run interactively" << endl
		 << " help      - displays usage info." << endl;
}

extern TDHCPClient * ptr;

BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
  switch( fdwCtrlType ) 
  { 
    // Handle the CTRL-C signal. 
    case CTRL_C_EVENT: 
	  ptr->stop();
      return( TRUE );
    case CTRL_BREAK_EVENT: 
      return FALSE; 
  }
}

int main(int argc, char* argv[])
{
    //Create the service object
    TClntService Client;
    int i=0;
    WSADATA wsaData;

	clog << DIBBLER_COPYRIGHT1 << " (CLIENT)" << endl;
	clog << DIBBLER_COPYRIGHT2 << endl;
	clog << DIBBLER_COPYRIGHT3 << endl;
	clog << DIBBLER_COPYRIGHT4 << endl;
	clog << endl;

	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );

	if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData )) {
        cout << "Unable to load WinSock 2.2 library." << endl;
		return -1;
	}

	EServiceState status = Client.ParseStandardArgs(argc, argv);
	Client.setState(status);

	// find ipv6.exe (or netsh.exe in future implementations)
	if (!lowlevelInit()) {
		clog << "lowlevelInit() failed. Startup aborted." << endl;
		return -1;		
	}

	switch(status) {
	case STATUS: { // STATUS
		Client.showStatus();
		break;
	}
	case START: { // START
		clog << "Running client... " << std::endl;
		Client.StartService();
		break;
	}
	case STOP: { // STOP
		// FIXME
		break;
	}
	case INSTALL: { // INSTALL
		Client.Install();
		break;
	}
	case UNINSTALL: { // UNINSTALL
		if (Client.IsInstalled())
			Client.Uninstall();
		else {
			clog << "Client already uninstalled." << endl;
			break;
		}
		clog << "Client unistalled." << endl;
		break;
	}
	case RUN: { // RUN
		clog << "Running client... (in console) " << std::endl;
		Client.Run();
		break;
	}
	case INVALID: {
		clog << endl << "Type: \"" << argv[0] << " help\" (without quotes) to see usage information." << endl;
		break;
    }				  
	case HELP: {
		usage();
    }
	default: {
		
	}
	
	}
	//When we get here, the service has been stopped
    //  return Client.Status.dwWin32ExitCode;
    //Client.Run();
    //clog<<logger::logInfo<<"Heap:"<<_CrtCheckMemory()<<endl;
	return 0;
}