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
		 << " run       - run interactively" << endl;

}

int main(int argc, char* argv[])
{
    //Create the service object
    TClntService Client;
    int i=0;
    WSADATA wsaData;
    if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ))
        cout<<"Nie mozna zaladowac biblioteki WinSock 2.2"<<endl;

	int status = Client.ParseStandardArgs(argc, argv);

	// find ipv6.exe (or netsh.exe in future implementations)
	if (!lowlevelInit()) {
		clog << "lowlevelInit() failed. Startup aborted.\n";
		return -1;		
	}

	switch(status) {
	case 1: { // STATUS
		clog << DIBBLER_COPYRIGHT1 << endl;
		clog << DIBBLER_COPYRIGHT2 << endl;
		clog << DIBBLER_COPYRIGHT3 << endl;
		clog << DIBBLER_COPYRIGHT4 << endl;
		clog << endl;
		clog << "Service: ";
		if (Client.IsInstalled()) {
			clog << "INSTALLED" << std::endl;
		} else {
			clog << "NOT INSTALLED" << std::endl;
		}
		// clog << "Running: "<< (Client.isRunning() ? "YES":"NO") << endl;
		break;
	}
	case 2: { // START
		clog << "Running client... " << std::endl;
		Client.StartService();
		break;
	}
	case 3: { // STOP
		// FIXME
		break;
	}
	case 4: { // INSTALL
		Client.Install();
		break;
	}
	case 5: { // UNINSTALL
		Client.Uninstall();
		break;
	}
	case 6: { // RUN
		clog << "Running client... (in console) " << std::endl;
		Client.Run();
		break;
	}
	default: {
		usage();
	}
	
	}
	//When we get here, the service has been stopped
    //  return Client.Status.dwWin32ExitCode;
    //Client.Run();
    //clog<<logger::logInfo<<"Heap:"<<_CrtCheckMemory()<<endl;
	return 0;
}