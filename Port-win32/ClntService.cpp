#include "ClntService.h"
#include "DHCPClient.h"
#include "portable.h"
#include "logger.h"
#include "DHCPConst.h"
#ifdef  WIN32
#include <crtdbg.h>
#endif

TClntService::TClntService() 
  :TWinService("DHCPv6Client","Klient DHCPv6",SERVICE_AUTO_START,"RpcSS\0tcpip6\0winmgmt\0")
{
}

int TClntService::ParseStandardArgs(int argc,char* argv[])
{    
    bool dirFound=false;
	bool ipFound=true;
	int status=1;
	int n=1;
        
	while(n<argc)
    {
		if (!strncmp(argv[n], "status",6))    {	return 1;}
		if (!strncmp(argv[n], "start",5))     {	status = 2;	}
		if (!strncmp(argv[n], "stop",4))      {	status = 3;	}
		if (!strncmp(argv[n], "install",7))   {	status = 4;	}
		if (!strncmp(argv[n], "uninstall",9)) {	return 5;}
		if (!strncmp(argv[n], "run",3))       { status = 6; }

		if (strncmp(argv[n], "-d",2)==0) {
			if (n+1==argc) {
				cout << "-d is a last parameter (additional filepath required)" << endl;
				return -1; // this is last parameter
			}
			n++;

			char temp[255];
			strncpy(temp,argv[n],255);
            ServiceDir=temp;
			cout << "workdir found: [" << temp << "]" << endl;
            dirFound=true;
        }
		n++;
    }
	if (dirFound)
		return status;
	else
		return -1;
}

void TClntService::OnShutdown()
{
    clog<<logger::logInfo<<"Service shutdown."<<std::endl;
    int fd=sock_add("",1,"::1",DHCPCLIENT_PORT+1005,true);
    char buf[4]={CONTROL_MSG,0,0,0};
    sock_send(fd,"::1",buf,4,DHCPCLIENT_PORT+1000,1);
    sock_del(fd);
}

void TClntService::OnStop()
{
    clog<<logger::logInfo<<"Stopping service."<<std::endl;
    int fd=sock_add("",1,"::1",DHCPCLIENT_PORT+1005,true);
    char buf[4]={CONTROL_MSG,0,0,0};
    sock_send(fd,"::1",buf,4,DHCPCLIENT_PORT+1000,1);
    sock_del(fd);
}

void TClntService::Run()
{
    // When we get here, the service has been stopped
	//return Client.Status.dwWin32ExitCode;
    string confile=CLNTCONF_FILE;
    string workdir=this->ServiceDir;
	string oldconf=CLNTCONF_FILE+(string)"old";
	string addrfile=CLNTDB_FILE;
    string logFile=workdir + CLNTLOG_FILE;
    char *logChar=new char[logFile.length()+1];
    strcpy(logChar,logFile.c_str());
    logger::Initialize(logChar);
    delete logChar;
    logger::setLogname("DHCPv6 Server");
    //char dir[200];
    //_getcwd(dir,200);
    TDHCPClient client(workdir+confile);
    client.run();
}

void TClntService::Install() {
	if (this->IsInstalled()) {
		printf("Service already installed. Aborting.\n");
		return;
	}
	bool result = this->TWinService::Install();
	if (!result) {
		printf("Service installation failed.\n");
	}
}

void TClntService::Uninstall() {
	if (!this->IsInstalled()) {
		printf("Service not installed. Cannot uninstall\n");
		return;
	}
	bool result = this->TWinService::Uninstall();
	if (!result) {
		printf("Service uninstallation failed.\n");
	}
}



TClntService::~TClntService(void)
{
}
