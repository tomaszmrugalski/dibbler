/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *          win2k version by <sob@hisoftware.cz>
 *
 * based on SrvService.cpp,v 1.15 2005/03/08 00:43:48 thomson Exp
 *
 * $Id: SrvService.cpp,v 1.2 2005-07-24 16:00:03 thomson Exp $
 *
 * Released under GNU GPL v2 licence
 *
 */

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
    srvPtr->stop();
}

bool TSrvService::CheckAndInstall() {
  // NT4 does not have winmgmt...
  OSVERSIONINFO verinfo;
  verinfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
  if(GetVersionEx(&verinfo) && (verinfo.dwMajorVersion < 5)) {
    Dependencies="RpcSS\0tcpip6\0";
  }
  return Install();
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
    
    Log(Crit) << DIBBLER_COPYRIGHT1 << " (SERVER, WinNT/2000 port)" << LogEnd;
    
    string tmp1 = workdir+"\\"+confile;
    TDHCPServer server(tmp1);
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
 * Revision 1.1  2005/07/23 14:33:22  thomson
 * Port for win2k/NT added.
 *
 */
 
