/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: ClntService.cpp,v 1.24 2008-08-30 20:41:06 thomson Exp $
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <winsock2.h>
#include "ClntService.h"
#include "DHCPClient.h"
#include "portable.h"
#include "logger.h"
#include "DHCPConst.h"
#include <direct.h>

TDHCPClient * clntPtr;
TClntService StaticService;

using namespace std;

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
    EServiceState status = INVALID;
    int n=1;
    
    while(n<argc) {
		if (!strncmp(argv[n], "status",6))    {	return STATUS;}
		if (!strncmp(argv[n], "start",5))     {	return START;	}
		if (!strncmp(argv[n], "stop",4))      {	return STOP;	}
		if (!strncmp(argv[n], "help",4))      {	return HELP;	}
		if (!strncmp(argv[n], "install",7))   {	status = INSTALL;	}
		if (!strncmp(argv[n], "uninstall",9)) {	return UNINSTALL;}
		if (!strncmp(argv[n], "run",3))       { status = RUN; }
        if (!strncmp(argv[n], "service", 7))  { status = SERVICE; }
	
		if (strncmp(argv[n], "-d",2)==0) {
			if (n+1==argc) {
				std::cout << "-d is a last parameter (additional filepath required)" << std::endl;
				return INVALID; // this is last parameter
			}
			n++;
			char temp[255];
			size_t endpos;
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
     // Log(Notice) << "-d parameter missing. Trying to load client.conf from current directory." << LogEnd;
        ServiceDir=".";
        return status;
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
    clntPtr->stop();
}

void TClntService::OnStop() {
	clntPtr->stop();
}

void TClntService::Run() {
    if (_chdir(this->ServiceDir.c_str())) {
	Log(Crit) << "Unable to change directory to " 
		  << this->ServiceDir << ". Aborting.\n" << LogEnd;
	return;
    }
    string confile  = CLNTCONF_FILE;
    string oldconf  = CLNTCONF_FILE+(string)"-old";
    string workdir  = this->ServiceDir;
    string addrfile = CLNTADDRMGR_FILE;
    string logFile  = CLNTLOG_FILE;
    
    logger::setLogName("Client");
	logger::Initialize((char*)logFile.c_str());
    
    Log(Crit) << DIBBLER_COPYRIGHT1 << "(CLIENT, WinXP/2003/Vista port)" << LogEnd;
    
    TDHCPClient client(workdir+"\\"+confile);
    clntPtr = &client; // remember address
    client.setWorkdir(this->ServiceDir);

    if (!client.isDone()) 
        client.run();
}

void TClntService::setState(EServiceState status) {
    this->status = status;
}

TClntService::~TClntService(void)
{
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.23  2006-05-01 13:56:28  thomson
 * winsock2.h include added.
 *
 * Revision 1.22  2005/08/07 18:10:59  thomson
 * 0.4.1 release.
 *
 * Revision 1.21  2005/07/24 16:00:02  thomson
 * Port WinNT/2000 related changes.
 *
 * Revision 1.20  2005/07/17 21:09:52  thomson
 * Minor improvements for 0.4.1 release.
 *
 * Revision 1.19  2005/06/07 21:58:49  thomson
 * 0.4.1
 *
 * Revision 1.18  2005/02/01 22:39:20  thomson
 * Command line service support greatly improved.
 *
 * Revision 1.17  2005/02/01 01:10:52  thomson
 * Working directory fix.
 *
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
 */
