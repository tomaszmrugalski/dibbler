#include <string>
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#include "Portable.h"
#include "DHCPServer.h"

#include "WinService.h"
#include "SrvService.h"
#include "Logger.h"

time_t timer;

void usage() {
	cout << "Usage:" << endl;
	cout << " dibbler-server.exe ACTION -d dirname -i ipv6path" << endl
		 << " ACTION = status|start|stop|install|uninstall|run" << endl
		 << " status    - show status and exit" << endl
		 << " start     - start installed service" << endl
		 << " stop      - stop installed service" << endl
		 << " install   - install service" << endl
		 << " uninstall - uninstall service" << endl
		 << " run       - run interactively" << endl;

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

	int status = SrvService.ParseStandardArgs(argc, argv);
	cout << "status=" << status << endl;

	switch (status) {
case 1: {// status
	clog << DIBBLER_COPYRIGHT1 << endl;
	clog << DIBBLER_COPYRIGHT2 << endl;
	clog << DIBBLER_COPYRIGHT3 << endl;
	clog << DIBBLER_COPYRIGHT4 << endl;
	clog << endl;
	clog << "Service: ";
	if (SrvService.IsInstalled()) 
		clog << "INSTALLED ";
	else
		clog << "NOT INSTALLED";

	clog << endl;
	break;
		};
case 2: { // start
	SrvService.Run();
	break;
		}

case 3: {// stop
	break;
		}
case 4: {// install
	SrvService.Install();
	break;
		}
case 5: {// uninstall
	SrvService.Uninstall();
	break;
	}
case 6: {// run
	SrvService.Run();
	break;
	}
default: {
	usage();
	}
}
    return 0;
}