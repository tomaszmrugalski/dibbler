/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: SrvService.cpp,v 1.20 2008-08-30 20:41:06 thomson Exp $
 */

#include <winsock2.h>
#include <direct.h>
#include "SrvService.h"
#include "DHCPClient.h"
#include "Logger.h"
#include "DHCPConst.h"

TDHCPServer * srvPtr;
TSrvService StaticService;

TSrvService::TSrvService() 
 :TWinService("DHCPv6Server","Dibbler - a DHCPv6 server",SERVICE_AUTO_START,
	      "RpcSS\0tcpip6\0winmgmt\0",
	      "Dibbler - a portable DHCPv6. This is DHCPv6 server, version "
	      DIBBLER_VERSION ".")
{
}

EServiceState TSrvService::ParseStandardArgs(int argc,char* argv[])
{
    bool dirFound = false;
    EServiceState status = INVALID;
    
    int n=1;
    while (n<argc)
    {
	    if (!strncmp(argv[n], "status",6))    {	return STATUS;}
	    if (!strncmp(argv[n], "start",5))     {	return START;	}
	    if (!strncmp(argv[n], "stop",4))      {	return STOP;	}
	    if (!strncmp(argv[n], "help",4))      { return HELP; }
	    if (!strncmp(argv[n], "uninstall",9)) {	return UNINSTALL; }
	    if (!strncmp(argv[n], "install",7))   {	status = INSTALL; }
	    if (!strncmp(argv[n], "run",3))       { status = RUN; }
        if (!strncmp(argv[n], "service", 7))  { status = SERVICE; }
    	
	    if (strncmp(argv[n], "-d",2)==0) {
	        if (n+1==argc) {
		        cout << "-d is a last parameter (additional filepath required)" << endl;
		        return INVALID; // this is last parameter
	        }
	        n++;
	        char temp[255];
	        strncpy(temp,argv[n],254);
		    int endpos = strlen(temp);
		    if (temp[endpos-1]=='\\')
			    temp[endpos-1]=0;
            ServiceDir = temp;
            dirFound=true;
        }
	    n++;
    }
    if (!dirFound) {
     // Log(Notice) << "-d parameter missing. Trying to load server.conf from current directory." << LogEnd;
        ServiceDir=".";
        return status;
    }

	if (ServiceDir.length()<3) {
	    Log(Crit) << "Invalid directory [" << ServiceDir << "]: too short. " 
		      << "Please use full absolute pathname, e.g. C:\\dibbler" << LogEnd;
	    return INVALID;
	}
	const char * tmp = ServiceDir.c_str();
    if ( tmp[0]=='.' && tmp[1]=='.')
        return status; // relative directory
    if ( tmp[0]=='\\' && tmp[1]=='\\')
    {
        Log(Warning) << "Network directory specified as config file location." << LogEnd;
        return status;
    }
    
    if ( *(tmp+1)!=':') {
	    Log(Notice) << "Invalid or not absolute directory [" << ServiceDir << "]: second character is not a ':'. "
		      << "Please use full absolute" << " pathname, e.g. C:\\dibbler" << LogEnd;
	}
	if ( *(tmp+2)!='\\' && *(tmp+2)!='/' ) {
	    Log(Notice) << "Invalid or absolute directory [" << ServiceDir << "]: third character is not a '/' nor '\\'. "
		      << "Please use full absolute" << " pathname, e.g. C:\\dibbler" << LogEnd;
	}
	return status;
}

TSrvService::~TSrvService(void)
{
}

void TSrvService::OnStop()
{
    srvPtr->stop();
}

void TSrvService::Run()
{
    if (_chdir(this->ServiceDir.c_str())) {
        Log(Crit) << "Unable to change directory to " 
	          << this->ServiceDir << ". Aborting.\n" << LogEnd;
        return;
    }
    
    string confile  = SRVCONF_FILE;
    string oldconf  = SRVCONF_FILE+(string)"-old";
    string workdir  = this->ServiceDir;
    string addrfile = SRVADDRMGR_FILE;
    string logFile  = SRVLOG_FILE;
    logger::setLogName("Srv");
	logger::Initialize((char*)logFile.c_str());
    
    Log(Crit) << DIBBLER_COPYRIGHT1 << " (SERVER, WinXP/2003/Vista/Win7 port)" << LogEnd;
    
    TDHCPServer server(confile);
    srvPtr = &server; // remember address
    server.setWorkdir(this->ServiceDir);
    
    if (!server.isDone())
	server.run();
}

void TSrvService::setState(EServiceState status) {
    this->status = status;
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.19  2006-05-01 14:03:28  thomson
 * winsock2.h include added.
 *
 * Revision 1.18  2005/07/24 16:00:03  thomson
 * Port WinNT/2000 related changes.
 *
 * Revision 1.17  2005/07/17 21:09:53  thomson
 * Minor improvements for 0.4.1 release.
 *
 * Revision 1.16  2005/06/07 21:58:49  thomson
 * 0.4.1
 *
 * Revision 1.15  2005/03/08 00:43:48  thomson
 * 0.4.0-RC2 release.
 *
 * Revision 1.14  2005/02/01 22:39:20  thomson
 * Command line service support greatly improved.
 *
 * Revision 1.13  2005/02/01 01:10:29  thomson
 * Files update.
 *
 * Revision 1.12  2004/12/13 23:31:30  thomson
 * *DB_FILE renamed to *ADDRMGR_FILE
 *
 * Revision 1.11  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.10  2004/12/02 00:51:06  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.9  2004/05/24 21:16:37  thomson
 * Various fixes.
 *
 * Revision 1.8  2004/04/15 23:24:43  thomson
 * Pathname installation fixed, run-time error checks disabled, winXP code cleanup.
 *
 * Revision 1.7  2004/03/29 21:37:07  thomson
 * 0.1.1 version
 *
 * Revision 1.6  2004/03/28 19:50:29  thomson
 * Problem with failed startup solved.
 *
 */
