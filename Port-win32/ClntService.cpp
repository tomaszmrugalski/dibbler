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
	// Depend on 'tcpip6' service only if it's Windows XP or Windows 2003.
	// Vista and above have IPv6 in the TCP stack and it's not a standalone service.
	OSVERSIONINFO verinfo;
	verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&verinfo);

	char dependenciesV5[] = "RpcSS\0tcpip6\0winmgmt\0";
	char dependenciesV6[] = "RpcSS\0winmgmt\0";
	if ( verinfo.dwMajorVersion <= 5 )
		memcpy(Dependencies, dependenciesV5, sizeof(dependenciesV5));
	else
		memcpy(Dependencies, dependenciesV6, sizeof(dependenciesV6));
}

EServiceState TClntService::ParseStandardArgs(int argc,char* argv[])
{
    bool dirFound = false;
    EServiceState status = INVALID;
    int n = 1;

    while(n<argc) {
        if (!strncmp(argv[n], "status",6)) {
            return STATUS;
        }
        if (!strncmp(argv[n], "start",5)) {
            return START;
        }
        if (!strncmp(argv[n], "stop",4)) {
            return STOP;
        }
        if (!strncmp(argv[n], "help",4)) {
            return HELP;
        }
        if (!strncmp(argv[n], "install",7)) {
            status = INSTALL;
        }
        if (!strncmp(argv[n], "uninstall",9)) {
            return UNINSTALL;
        }
        if (!strncmp(argv[n], "run",3)) {
            status = RUN;
        }
        if (!strncmp(argv[n], "service", 7)) {
            status = SERVICE;
        }

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
    if ( *(tmp + 1) != ':') {
        Log(Crit) << "Invalid directory [" << ServiceDir << "]: second character is not a ':'. "
                  << "Please use full absolute" << " pathname, e.g. C:\\dibbler" << LogEnd;
        return INVALID;
    }
    if ( *(tmp + 2) != '\\' && *(tmp + 2) != '/' ) {
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

    Log(Crit) << DIBBLER_COPYRIGHT1 << "(CLIENT, WinXP/2003/Vista/7/8 port)" << LogEnd;

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
