/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *          win2k version by <sob@hisoftware.cz>
 *
 * based on client-winxp.cpp,v 1.15 2005/02/01 22:39:20 thomson Exp $
 *
 * $Id: client-winnt2k.cpp,v 1.3 2005-07-26 00:03:03 thomson Exp $
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <string>
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h> 
#include <process.h>

#include "WinService.h"
#include "ClntService.h"
#include "Portable.h"
#include "DHCPClient.h"
#include "logger.h"
#include <iostream>

extern "C" int lowlevelInit();

using namespace std;

void usage() {
	cout << "Usage:" << endl;
	cout << " dibbler-client.exe ACTION [-d c:\\path\\to\\config\\file]" << endl
		 << " ACTION = status|start|stop|install|uninstall|run" << endl
		 << " status    - show status and exit" << endl
		 << " start     - start installed service" << endl
		 << " stop      - stop installed service" << endl
		 << " install   - install service" << endl
		 << " uninstall - uninstall service" << endl
		 << " run       - run interactively" << endl
		 << " help      - displays usage info." << endl << endl
		 << " Note: -d parameter is optional." << endl;
}

extern TDHCPClient * clntPtr;

/* 
 * Handle the CTRL-C, CTRL-BREAK signal. 
 */
BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
  switch( fdwCtrlType ) 
  { 
  case CTRL_C_EVENT: {
	  clntPtr->stop();
      return TRUE;
  }
    case CTRL_BREAK_EVENT: 
      return FALSE; 
  }
  return TRUE;
}

int main(int argc, char* argv[])
{
    // get the service object
    TClntService * Client = TClntService::getHandle();

    WSADATA wsaData;

	cout << DIBBLER_COPYRIGHT1 << " (CLIENT, WinNT/2000 port)" << endl;
	cout << DIBBLER_COPYRIGHT2 << endl;
	cout << DIBBLER_COPYRIGHT3 << endl;
	cout << DIBBLER_COPYRIGHT4 << endl;
	cout << endl;

	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );

	if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData )) {
        cout << "Unable to load WinSock 2.2 library." << endl;
		return -1;
	}

	EServiceState status = Client->ParseStandardArgs(argc, argv);
	Client->setState(status);
	if (!Client->verifyPort()) {
       Log(Crit) << "Operating system version is not supported by this Dibbler port." << LogEnd;
       return -1;
    }
	
	// find ipv6.exe (or netsh.exe in future implementations)
	if (!lowlevelInit()) {
		clog << "lowlevelInit() failed. Startup aborted." << endl;
		return -1;		
	}

	switch(status) {
	case STATUS: { 
		Client->showStatus();
		break;
	}
	case START: { 
		Client->StartService();
		break;
	}
	case STOP: { 
        Client->StopService();
        break;
	}
	case INSTALL: {
		Client->CheckAndInstall();
		break;
	}
	case UNINSTALL: {
		Client->Uninstall();
		break;
	}
	case RUN: {
		Client->Run();
		break;
	}
    case SERVICE: {
        Client->RunService();
        break;
    }
	case INVALID: {
		Log(Crit) << "Invalid usage." << endl;
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
