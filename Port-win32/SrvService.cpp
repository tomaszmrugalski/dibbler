/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * $Id: SrvService.cpp,v 1.8 2004-04-15 23:24:43 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2004/03/29 21:37:07  thomson
 * 0.1.1 version
 *
 * Revision 1.6  2004/03/28 19:50:29  thomson
 * Problem with failed startup solved.
 *
 *                                                                           
 * Released under GNU GPL v2 licence                                
 *                                                                           
 */

#include <direct.h>
#include "SrvService.h"
#include "DHCPClient.h"
#include "Logger.h"
#include "DHCPConst.h"
#ifdef  WIN32
#include <crtdbg.h>
#endif

TDHCPServer * ptr;

TSrvService::TSrvService() 
 :TWinService("DHCPv6Server","Dibbler - a DHCPv6 server",SERVICE_AUTO_START,
	"RpcSS\0tcpip6\0winmgmt\0",
	"Dibbler - a portable DHCPv6. This is DHCPv6 server,"
    "Windows version.")
{
}

int TSrvService::ParseStandardArgs(int argc,char* argv[])
{
	bool dirFound = false;
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
            ServiceDir = temp;
            dirFound=true;
        }

 		n++;
    }
	if (dirFound)
		return status;
	else
		return -1;
}

TSrvService::~TSrvService(void)
{
}

void TSrvService::OnStop()
{
	ptr->stop();
}

void TSrvService::Run()
{
	if (_chdir(this->ServiceDir.c_str())) {
		logger::clog << "Unable to change directory to " 
			<< this->ServiceDir << ". Aborting.\n" << logger::endl;
		return;
	}
   
    string confile  = SRVCONF_FILE;
	string oldconf  = SRVCONF_FILE+(string)"-old";
    string workdir  = this->ServiceDir;
	string addrfile = SRVDB_FILE;
    string logFile  = SRVLOG_FILE;
    logger::Initialize((char*)logFile.c_str());
	logger::setLogName("Srv");

	clog << logger::logCrit << DIBBLER_COPYRIGHT1 << " (SERVER)" << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT2 << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT3 << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT4 << endl;

	TDHCPServer server(confile);
	ptr = &server; // remember address

	if (!server.isDone())
       server.run();
}