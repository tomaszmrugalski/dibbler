/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * $Id: ClntService.cpp,v 1.17 2005-02-01 01:10:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.16  2004/12/13 23:31:30  thomson
 * *DB_FILE renamed to *ADDRMGR_FILE
 *
 * Revision 1.15  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.14  2004/12/02 00:51:06  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.13  2004/05/24 21:16:37  thomson
 * Various fixes.
 *
 * Revision 1.12  2004/04/15 23:24:43  thomson
 * Pathname installation fixed, run-time error checks disabled, winXP code cleanup.
 *
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
		 " version " DIBBLER_VERSION ".")
{
}

EServiceState TClntService::ParseStandardArgs(int argc,char* argv[])
{    
    bool dirFound=false;
    EServiceState status=STATUS;
    int n=1;
    
    while(n<argc) {
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
    if (!dirFound) {
		return INVALID;
	}
	
	if (ServiceDir.length()<3) {
	    Log(Crit) << "Invalid directory [" << ServiceDir 
		      << "]: too short. Please use full absolute pathname, e.g. C:\\dibbler"
		      << LogEnd;
	    return INVALID;
	}
	const char * tmp = ServiceDir.c_str();
	if ( *(tmp+1)!=':') {
	    Log(Crit) << "Invalid directory [" << ServiceDir << "]: second character is not a ':'. " 
		      << "Please use full absolute" << " pathname, e.g. C:\\dibbler" << LogEnd;
	    return INVALID;
	}
	if ( *(tmp+2)!='\\' && *(tmp+2)!='/' ) {
	    Log(Crit) << "Invalid directory [" << ServiceDir << "]: third character is not a '/' nor '\\'. "
		      << "Please use full absolute" << " pathname, e.g. C:\\dibbler" << LogEnd;
	    return INVALID;
	}
	return status;
}

void TClntService::OnShutdown() {
    ptr->stop();
}

void TClntService::OnStop() {
	ptr->stop();
}

void TClntService::Run() {
    if (_chdir(this->ServiceDir.c_str())) {
	Log(Crit) << "Unable to change directory to " 
		  << this->ServiceDir << ". Aborting.\n" << LogEnd;
	return;
    }
    string confile  = CLNTCONF_FILE;
    string oldconf  = CLNTCONF_FILE+(string)"-old";
    string addrfile = CLNTADDRMGR_FILE;
    string logFile  = CLNTLOG_FILE;
    
    logger::setLogName("Client");
	logger::Initialize((char*)logFile.c_str());
    
    Log(Crit) << DIBBLER_COPYRIGHT1 << "(CLIENT)" << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT2 << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT3 << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT4 << LogEnd;
    
    TDHCPClient client(confile);
    ptr = &client; // remember address
    client.setWorkdir(this->ServiceDir);

    if (!client.isDone()) 
        client.run();
}

void TClntService::Install() {
    if (this->IsInstalled()) {
	cout << "Service already installed. Aborting." << endl;
	return;
    }
    bool result = this->TWinService::Install();
    if (!result) {
	cout << "Service installation failed." << endl;
    } else {
	cout << "Installation successful." << endl;
    }
}

void TClntService::Uninstall() {
    if (!this->IsInstalled()) {
	cout << "Service not installed. Cannot uninstall" << endl;
	return;
    }
    bool result = this->TWinService::Uninstall();
    if (!result) {
	cout << "Service uninstallation failed." << endl;
    }
}

void TClntService::setState(EServiceState status) {
    this->status = status;
}

TClntService::~TClntService(void)
{
}
