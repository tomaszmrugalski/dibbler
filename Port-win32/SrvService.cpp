#include <direct.h>
#include "SrvService.h"
#include "DHCPClient.h"
#include "Logger.h"
#include "DHCPConst.h"
#ifdef  WIN32
#include <crtdbg.h>
#endif

char * ipv6Path;

TSrvService::TSrvService() : TWinService("Dibbler-server","DHCPv6 server",SERVICE_AUTO_START,"RpcSS\0tcpip6\0winmgmt\0")
{
}

int TSrvService::ParseStandardArgs(int argc,char* argv[])
{
	bool dirFound = false;
    bool ipFound  = false;
	int status = 1;

	int n=1;
    while (n<argc)
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

        if (strncmp(argv[n], "-i",2)==0) {
           ipv6Path= new char [255];
           ipv6Path[0]=0;
		   if (n+1==argc) 
				return -1; // this is last parameter
		   n++;
           strcat(ipv6Path,argv[n]);
           ipv6Path[strlen(ipv6Path)]=0;
           ipFound=true;
		   cout << "ipv6path found: [" << ipv6Path << "]" << endl;
        }
		n++;
    }
	if (ipFound && dirFound)
		return status;
	else
		return -1;
}

TSrvService::~TSrvService(void)
{
}

void TSrvService::OnStop()
{
    clog<<logger::logInfo<<"Stopping service."<<std::endl;
    int fd=sock_add("",1,"::1",DHCPSERVER_PORT+1005,true);
    char buf[4]={CONTROL_MSG,0,0,0};
    sock_send(fd,"::1",buf,4,DHCPSERVER_PORT+1000,1);
    sock_del(fd);
}

void TSrvService::Run()
{
    // When we get here, the service has been stopped
	//return Client.Status.dwWin32ExitCode;
    
    string confile=SRVCONF_FILE;

    string workdir=this->ServiceDir;
	string oldconf=SRVCONF_FILE+(string)"-old";
	string addrfile=SRVDB_FILE;
    string logFile=workdir + SRVLOG_FILE;
    char *logChar=new char[logFile.length()+1];
    strcpy(logChar,logFile.c_str());
    logger::Initialize(logChar);
    delete logChar;
    //clog<<logger::logAlert<<"Working dir:"<<ServiceDir;
    logger::setLogname("Dibbler server");
    char dir[200];
    _getcwd(dir,200);
    TDHCPServer server(workdir+confile);
    server.run();
}