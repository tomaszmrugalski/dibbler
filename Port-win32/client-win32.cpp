/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: client-win32.cpp,v 1.18 2005-07-26 00:03:03 thomson Exp $
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

	cout << DIBBLER_COPYRIGHT1 << " (CLIENT, WinXP/2003 port)" << endl;
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

    // is this proper port?
	if (!RelService->verifyPort()) {
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
		Client->Install();
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
 * Revision 1.17  2005/07/24 16:00:03  thomson
 * Port WinNT/2000 related changes.
 *
 * Revision 1.16  2005/07/17 21:09:53  thomson
 * Minor improvements for 0.4.1 release.
 *
 * Revision 1.15  2005/02/01 22:39:20  thomson
 * Command line service support greatly improved.
 *
 * Revision 1.14  2004/10/03 21:28:44  thomson
 * 0.2.1-RC1 version.
 *
 * Revision 1.13  2004/09/28 21:49:32  thomson
 * no message
 *
 * Revision 1.12  2004/07/17 16:02:34  thomson
 * Command line minor modifications.
 *
 * Revision 1.11  2004/05/24 21:16:37  thomson
 * Various fixes.
 *
 * Revision 1.10  2004/04/15 23:53:45  thomson
 * Pathname installation fixed, run-time error checks disabled, winXP code cleanup.
 *
 * Revision 1.9  2004/04/15 23:24:44  thomson
 * Pathname installation fixed, run-time error checks disabled, winXP code cleanup.
 *
 * Revision 1.8  2004/03/29 22:06:49  thomson
 * 0.1.1 version
 */
