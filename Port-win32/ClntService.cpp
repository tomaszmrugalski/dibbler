/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * $Id: ClntService.cpp,v 1.12 2004-04-15 23:24:43 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2004/04/12 22:21:29  thomson
 * setLogname changed to setLogName
 *
 * Revision 1.10  2004/03/29 21:37:07  thomson
 * 0.1.1 version
 *
 * Revision 1.9  2004/03/28 19:50:19  thomson
 * Problem with failed startup solved.
 *
 *
 * Released under GNU GPL v2 licence
 *
 */

#include "ClntService.h"
#include "DHCPClient.h"
#include "portable.h"
#include "logger.h"
#include "DHCPConst.h"
#include <crtdbg.h>
#include <direct.h>

TDHCPClient * ptr;

TClntService::TClntService() 
  :TWinService("DHCPv6Client","Dibbler - a DHCPv6 client",SERVICE_AUTO_START,
  "tcpip6\0winmgmt\0",
  "Dibbler - a portable DHCPv6. This is DHCPv6 client,"
  " version " DIBBLER_VERSION ",WindowsXP.")
{
}

EServiceState TClntService::ParseStandardArgs(int argc,char* argv[])
{    
    bool dirFound=false;
	EServiceState status=STATUS;
	int n=1;
        
	while(n<argc)
    {
		if (!strncmp(argv[n], "status",6))    {	return STATUS;}
		if (!strncmp(argv[n], "start",5))     {	status = START;	}
		if (!strncmp(argv[n], "stop",4))      {	status = STOP;	}
		if (!strncmp(argv[n], "help",4))      {	return HELP;	}
		if (!strncmp(argv[n], "install",7))   {	status = INSTALL;	}
		if (!strncmp(argv[n], "uninstall",9)) {	return UNINSTALL;}
		if (!strncmp(argv[n], "run",3))       { status = RUN; }

		if (strncmp(argv[n], "-d",2)==0) {
			if (n+1==argc) {
				cout << "-d is a last parameter (additional filepath required)" << endl;
				return INVALID; // this is last parameter
			}
			n++;

			char temp[255];
			int endpos;
			strncpy(temp,argv[n],255);
			endpos = strlen(temp);
			if (temp[endpos-1]=='\\')
				temp[endpos-1]=0;
            ServiceDir=temp;
            dirFound=true;
        }
		n++;
    }
	if (dirFound) {
		if (ServiceDir.length()<3) {
			cout << "Invalid directory [" << ServiceDir 
				<< "]: too short. " << endl << "Please use full absolute pathname, e.g. C:\\dibbler"
				<< logger::endl;
			return INVALID;
		}
		const char * tmp = ServiceDir.c_str();
		if ( *(tmp+1)!=':') {
			cout << "Invalid directory [" << ServiceDir
				<< "]: second character is not a ':'. " << endl << "Please use full absolute"
				" pathname, e.g. C:\\dibbler" << logger::endl;
			return INVALID;
		}
		if ( *(tmp+2)!='\\' && *(tmp+2)!='/' ) {
			cout << "Invalid directory [" << ServiceDir
				<< "]: third character is not a '/' nor '\\'. " << endl << "Please use full absolute"
				" pathname, e.g. C:\\dibbler" << logger::endl;
			return INVALID;
		}
		return status;
	} 
	else {
		return INVALID;
	}
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

	logger::setLogName("Client");
	if (this->status != RUN)
      logger::Initialize((char*)logFile.c_str());

	clog << logger::logCrit << DIBBLER_COPYRIGHT1 << "(CLIENT)" << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT2 << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT3 << endl;
	clog << logger::logCrit << DIBBLER_COPYRIGHT4 << endl;

	TDHCPClient client(confile);
	ptr = &client; // remember address

	if (!client.isDone()) 
        client.run();
}

void TClntService::Install() {
	if (this->IsInstalled()) {
		clog << "Service already installed. Aborting." << endl;
		return;
	}
	bool result = this->TWinService::Install();
	if (!result) {
		clog << "Service installation failed." << endl;
	} else {
		clog << "Installation successful." << endl;
	}
}

void TClntService::Uninstall() {
	if (!this->IsInstalled()) {
		clog << "Service not installed. Cannot uninstall" << endl;
		return;
	}
	bool result = this->TWinService::Uninstall();
	if (!result) {
		clog << "Service uninstallation failed." << endl;
	}
}

void TClntService::setState(EServiceState status) {
	this->status = status;
}

TClntService::~TClntService(void)
{
}
