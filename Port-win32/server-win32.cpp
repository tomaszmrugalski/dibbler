/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Hernan Martinez <hernan(dot)c(dot)martinez(at)gmail(dot)com>
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

using namespace std;

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

    cout << DIBBLER_COPYRIGHT1 << " (SERVER, WinXP/2003/Vista/7/8 port)" << endl;
    cout << DIBBLER_COPYRIGHT2 << endl;
    cout << DIBBLER_COPYRIGHT3 << endl;
    cout << DIBBLER_COPYRIGHT4 << endl;
    cout << endl;

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

    // is this proper port?
    if (!SrvService->verifyPort()) {
        Log(Crit) << "Operating system version is not supported by this Dibbler port."
                  << LogEnd;
       return -1;
    }

    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );

    // Check for administrative privileges for some of the actions
    switch ( status ) {
    case START:
    case STOP:
    case INSTALL:
    case UNINSTALL:
        if( !SrvService->isRunAsAdmin() ) {
            Log(Crit) << SrvService->ADMIN_REQUIRED_STR << LogEnd;
            return -1;
        }
        break;
    default:
        break;
    }

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
        Log(Crit) << "Invalid usage." << LogEnd;
        // No break here; fall through to help
    }
    case HELP:
    default: {
        usage();
    }
    }

    return 0;
}
