#include <string>
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#include "WinService.h"
#include "ClnService.h"
#include "Portable.h"
#include "DHCPClient.h"
#include "logger.h"
#include <iostream>
#ifdef  WIN32
#include <crtdbg.h>
#endif

time_t timer; 

using namespace std;
int main(int argc, char* argv[])
{
    //Create the service object
    TClntService Client;
    int i=0;
    WSADATA wsaData;
    if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ))
        cout<<"Nie mozna zaladowac biblioteki WinSock 2.2"<<endl;
    //Client.ParseStandardArgs(argc,argv);
    if (!(Client.ParseStandardArgs(argc, argv)))
      Client.StartService();
    //When we get here, the service has been stopped
    //  return Client.Status.dwWin32ExitCode;
    //Client.Run();
    //clog<<logger::logInfo<<"Heap:"<<_CrtCheckMemory()<<endl;

}