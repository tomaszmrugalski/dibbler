/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *          win2k version by <sob@hisoftware.cz>
 *
 * based on relay-win32.cpp,v 1.4 2005/02/01 22:39:20 thomson Exp $
 *
 * $Id: relay-winnt2k.cpp,v 1.4 2007-05-05 15:40:33 thomson Exp $
 *
 * Released under GNU GPL v2 licence
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
extern TDHCPRelay * relPtr;

void usage() {
	cout << "Usage:" << endl;
	cout << " dibbler-relay.exe ACTION [-d c:\\path\\to\\config\\file]" << endl
		 << " ACTION = status|start|stop|install|uninstall|run" << endl
		 << " status    - show status and exit" << endl
		 << " start     - start installed service" << endl
		 << " stop      - stop installed service" << endl
		 << " install   - install service" << endl
		 << " uninstall - uninstall service" << endl
		 << " run       - run in console" << endl 
		 << endl 
		 << " -d parameter is optional." << endl;
}

/* 
 * Handle the CTRL-C, CTRL-BREAK signal. 
 */
BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
  switch( fdwCtrlType ) 
  { 
  case CTRL_C_EVENT: {
	  relPtr->stop();
      return TRUE;
  }
    case CTRL_BREAK_EVENT: 
      return FALSE; 
  }
  return TRUE;
}

int main(int argc, char* argv[]) {

    cout << DIBBLER_COPYRIGHT1 << " (RELAY, WinNT/2000 port)" << endl;
    cout << DIBBLER_COPYRIGHT2 << endl;
    cout << DIBBLER_COPYRIGHT3 << endl;
    cout << DIBBLER_COPYRIGHT4 << endl;
    cout << endl;
	
	// get the service object
    TRelService * RelService = TRelService::getHandle();

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
    
    EServiceState status = RelService->ParseStandardArgs(argc, argv);
    RelService->setState(status);
    
    // is this proper port?
	if (!RelService->verifyPort()) {
       Log(Crit) << "Operating system version is not supported by this Dibbler port." << LogEnd;
       return -1;
    }
        
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
    
    switch (status) {
    case STATUS: {
        RelService->showStatus();
	    break;
    };
    case START: {
	    RelService->StartService();
	    break;
    }
    case STOP: {
	    RelService->StopService();
    	break;
    }
    case INSTALL: {
        RelService->Install();
	    break;
    }
    case UNINSTALL: {
	    RelService->Uninstall();
	    break;
    } 
    case RUN: {
	    RelService->Run();
	    break;
    }
    case SERVICE: {
        RelService->RunService();
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

/* ugly workaround for some weird dev-cpp/mingw 3.4.2 problem */
void md5_init_ctx (struct md5_ctx *ctx) { }
void md5_process_bytes (const void *buffer, size_t len, struct md5_ctx *ctx) { }
void *md5_finish_ctx (struct md5_ctx *ctx, void *resbuf) { }
