#include "ClntService.h"
#include "DHCPClient.h"
#include "portable.h"
#include "logger.h"
#include "DHCPConst.h"
#include <crtdbg.h>
#include <direct.h>

TDHCPClient * ptr;

TClntService::TClntService() 
  :TWinService("DHCPv6Client","Dibbler - a DHCPv6 client",SERVICE_AUTO_START,"RpcSS\0tcpip6\0winmgmt\0")
{
}

int TClntService::ParseStandardArgs(int argc,char* argv[])
{    
    bool dirFound=false;
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
	ptr->stop();
}

void TClntService::OnStop()
{
	ptr->stop();
}

void TClntService::Run()
{
	if (_chdir(this->ServiceDir.c_str())) {
		logger::clog << "Unable to change directory to " 
			<< this->ServiceDir << ". Aborting.\n" << logger::endl;
		return;
	}
    string confile  = CLNTCONF_FILE;
	string oldconf  = CLNTCONF_FILE+(string)"-old";
	string addrfile = CLNTDB_FILE;
    string logFile  = CLNTLOG_FILE;
    logger::Initialize((char*)logFile.c_str());

	TDHCPClient client(confile);
	ptr = &client; // remember address

	clog << logger::logCrit << DIBBLER_COPYRIGHT1 << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT2 << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT3 << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT4 << endl;

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
