/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *
 * Released under GNU GPL v2 licence                                
 *
 * $Id: RelService.cpp,v 1.3 2005-02-01 22:08:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/01 01:11:10  thomson
 * Working directory fix.
 *
 * Revision 1.1  2005/01/24 00:42:37  thomson
 * no message
 *
 *                                                                           
 */

#include <direct.h>
#include "RelService.h"
#include "DHCPRelay.h"
#include "Portable.h"
#include "Logger.h"
#include "DHCPConst.h"
#ifdef  WIN32
#include <crtdbg.h>
#endif

TDHCPRelay * ptr;
TRelService StaticService;

TRelService::TRelService() 
 :TWinService("DHCPv6Relay","Dibbler - a DHCPv6 relay",SERVICE_AUTO_START,
	"RpcSS\0tcpip6\0winmgmt\0",
	"Dibbler - a portable DHCPv6. This is DHCPv6 Relay, version "
    DIBBLER_VERSION ".")
{
}

EServiceState TRelService::ParseStandardArgs(int argc,char* argv[])
{
    bool dirFound = false;
    EServiceState status = STATUS;
    
    int n=1;
    while (n<argc)
    {
	if (!strncmp(argv[n], "status",6))    {	return STATUS;}
	if (!strncmp(argv[n], "start",5))     {	return START;	}
	if (!strncmp(argv[n], "stop",4))      {	return STOP;	}
	if (!strncmp(argv[n], "help",4))      { return HELP; }
	if (!strncmp(argv[n], "install",7))   {	status = INSTALL; }
	if (!strncmp(argv[n], "uninstall",9)) {	return UNINSTALL; }
	if (!strncmp(argv[n], "run",3))       { status = RUN; }
    if (!strncmp(argv[n], "service", 7))  { status = SERVICE; }
	
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

TRelService::~TRelService(void)
{
}

void TRelService::OnStop()
{
    ptr->stop();
}

void TRelService::Run()
{
    if (_chdir(this->ServiceDir.c_str())) {
	Log(Crit) << "Unable to change directory to " 
		  << this->ServiceDir << ". Aborting.\n" << LogEnd;
	return;
    }
    
    string confile  = RELCONF_FILE;
    string oldconf  = RELCONF_FILE+(string)"-old";
    string workdir  = this->ServiceDir;
    string logFile  = RELLOG_FILE;
    logger::setLogName("Rel");
	logger::Initialize((char*)logFile.c_str());
    
    Log(Crit) << DIBBLER_COPYRIGHT1 << " (RELAY)" << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT2 << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT3 << LogEnd;
    Log(Crit) << DIBBLER_COPYRIGHT4 << LogEnd;
    
    TDHCPRelay relay(confile);
    ptr = &relay; // remember address
    relay.setWorkdir(this->ServiceDir);

    if (!relay.isDone())
	relay.run();
}

void TRelService::setState(EServiceState status) {
    this->status = status;
}

