/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * $Id: SrvService.cpp,v 1.11 2004-12-03 20:51:42 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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

EServiceState TSrvService::ParseStandardArgs(int argc,char* argv[])
{
    bool dirFound = false;
    EServiceState status = STATUS;
    
    int n=1;
    while (n<argc)
    {
	if (!strncmp(argv[n], "status",6))    {	return STATUS;}
	if (!strncmp(argv[n], "start",5))     {	status = START;	}
	if (!strncmp(argv[n], "stop",4))      {	status = STOP;	}
	if (!strncmp(argv[n], "help",4))      { return HELP; }
	if (!strncmp(argv[n], "install",7))   {	status = INSTALL; }
	if (!strncmp(argv[n], "uninstall",9)) {	return UNINSTALL; }
	if (!strncmp(argv[n], "run",3))       { status = RUN; }
	
	if (strncmp(argv[n], "-d",2)==0) {
	    if (n+1==argc) {
		cout << "-d is a last parameter (additional filepath required)" << endl;
		return INVALID; // this is last parameter
	    }
	    n++;
	    char temp[255];
	    strncpy(temp,argv[n],255);
		int endpos = strlen(temp);
		if (temp[endpos-1]=='\\')
			temp[endpos-1]=0;
        ServiceDir = temp;
        dirFound=true;
    }
	n++;
    }
    if (!dirFound)
		return INVALID;
	if (ServiceDir.length()<3) {
	    Log(Crit) << "Invalid directory [" << ServiceDir << "]: too short. " 
		      << "Please use full absolute pathname, e.g. C:\\dibbler" << LogEnd;
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
	Log(Crit) << "Unable to change directory to " 
		  << this->ServiceDir << ". Aborting.\n" << LogEnd;
	return;
    }
    
    string confile  = SRVCONF_FILE;
    string oldconf  = SRVCONF_FILE+(string)"-old";
    string workdir  = this->ServiceDir;
    string addrfile = SRVDB_FILE;
    string logFile  = SRVLOG_FILE;
    logger::setLogName("Srv");
	logger::Initialize((char*)logFile.c_str());
    
    Log(Crit) << DIBBLER_COPYRIGHT1 << " (SERVER)" << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT2 << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT3 << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT4 << LogEnd;
    
    TDHCPServer server(confile);
    ptr = &server; // remember address
    
    if (!server.isDone())
	server.run();
}

void TSrvService::setState(EServiceState status) {
    this->status = status;
}

