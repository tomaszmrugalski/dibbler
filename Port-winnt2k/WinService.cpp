/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *          win2k version by <sob@hisoftware.cz>
 *
 * based on WinService.cpp,v 1.14 2005/02/01 22:39:20 thomson Exp $
 *
 * $Id: WinService.cpp,v 1.4 2008-09-22 17:08:53 thomson Exp $
 *
 * Released under GNU GPL v2 licence
 *                                                                           
 */

#include <windows.h>
#include <stdio.h>
#include "winservice.h"
#include "Logger.h"

TWinService* TWinService::ServicePtr= NULL;

TWinService::TWinService(const char* serviceName, const char* dispName, 
			 DWORD serviceType, char* dependencies, char * descr)
{
    ServicePtr= this;
    strncpy(ServiceName, serviceName, sizeof(ServiceName)-1);
    ServiceType=serviceType;
    Dependencies=dependencies;
    MajorVersion = 1;
    MinorVersion = 0;
    EventSource = NULL;

	this->descr = descr;
    
    // set service status 
    hServiceStatus = (SERVICE_STATUS_HANDLE)NULL;
    Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    Status.dwCurrentState = SERVICE_STOPPED;
    Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    Status.dwWin32ExitCode = 0;
    Status.dwServiceSpecificExitCode = 0;
    Status.dwCheckPoint = 0;
    Status.dwWaitHint = 0;
    IsRunning = FALSE;
    DisplayName=new char[strlen(dispName)+1];
    strcpy(DisplayName,dispName);
}

TWinService::~TWinService(void)
{
    if (EventSource)
        DeregisterEventSource(EventSource);
}

void TWinService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Get a pointer to the C++ object
    TWinService* pService = ServicePtr;
    
    // Register the control request handler
    pService->Status.dwCurrentState = SERVICE_START_PENDING;
    pService->hServiceStatus = RegisterServiceCtrlHandler(pService->ServiceName,Handler);
    if (pService->hServiceStatus==(SERVICE_STATUS_HANDLE)NULL)
	{
        return;
    }
    // Start the initialisation
    if (pService->Initialize()) 
	{
        // Do the real work. 
        // When the Run function returns, the service has stopped.
        pService->IsRunning = TRUE;
        pService->Status.dwWin32ExitCode = 0;
        pService->Status.dwCheckPoint = 0;
        pService->Status.dwWaitHint = 0;
        pService->Run();
    }
    // Tell the service manager we are stopped
    pService->SetStatus(SERVICE_STOPPED);
}

void TWinService::Handler(DWORD dwOpcode) {
	//DebugBreak();
    // Get a pointer to the object
	TWinService* pService = ServicePtr;
    
    switch (dwOpcode) {
    case SERVICE_CONTROL_STOP: // 1
        pService->SetStatus(SERVICE_STOP_PENDING);
        pService->OnStop();
        pService->SetStatus(SERVICE_STOPPED);
        pService->IsRunning = FALSE;
        break;

    case SERVICE_CONTROL_PAUSE: // 2
        pService->OnPause();
        break;

    case SERVICE_CONTROL_CONTINUE: // 3
        pService->OnContinue();
        break;

    case SERVICE_CONTROL_INTERROGATE: // 4
        pService->OnInterrogate();
        break;

    case SERVICE_CONTROL_SHUTDOWN: // 5
        pService->OnShutdown();
        break;

    default:
        if (dwOpcode >= SERVICE_CONTROL_USER) 
		{
            if (!pService->OnUserControl(dwOpcode)) 
			{
            }
        } 
		else 
		{
        }
        break;
    }

    // Report current status
    SetServiceStatus(pService->hServiceStatus, &pService->Status);
}

void TWinService::LogEvent(WORD wType, DWORD dwID,
		          const char* pszS1,
                  const char* pszS2,
                  const char* pszS3)
{
    const char* ps[3];
    ps[0] = pszS1;
    ps[1] = pszS2;
    ps[2] = pszS3;
    int iStr = 0;
    for (int i = 0; i < 3; i++) 
        if (ps[i] != NULL) iStr++;      
    // Check the event source has been registered and if
    // not then register it now
    if (!EventSource)
        EventSource = ::RegisterEventSource(NULL,ServiceName); 
    if (EventSource) 
        ReportEvent(EventSource,wType,0,dwID,NULL,iStr,0,ps,NULL);
}

bool TWinService::IsInstalled()
{
    return this->IsInstalled(this->ServiceName);
}

bool TWinService::IsInstalled(const char *name)
{
    bool result = false;
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = OpenService(hSCM,name,SERVICE_QUERY_CONFIG);
        if (hService) {
            result = true;
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCM);
    }
    return result;
}


bool TWinService::Install()
{
    if (this->IsInstalled()) {
        Log(Crit) << "Service " << ServiceName << " is already installed." << LogEnd;
        return false;
    }

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return false;
    // Get the executable file path
    char filePath[_MAX_PATH];
    GetModuleFileName(NULL, filePath, sizeof(filePath));
	int i = strlen(filePath); 
	sprintf(filePath+i, " service -d \"%s\"",ServiceDir.c_str());

    // Create the service
	//printf("Install(): filepath=[%s]\nServiceName=[%s]\n",filePath,ServiceName);
	//printf("ServiceDir=[%s]\n",ServiceDir.c_str());
    SC_HANDLE hService = CreateService(	hSCM,ServiceName, DisplayName,
					SERVICE_ALL_ACCESS,
					SERVICE_WIN32_OWN_PROCESS,
					ServiceType,
                                        SERVICE_ERROR_NORMAL,
					filePath,NULL,NULL,Dependencies,NULL,NULL);
    if (!hService) 
    {
        CloseServiceHandle(hSCM);
        Log(Crit) << "Unable to create " << ServiceName << " service." << LogEnd;
        return FALSE;
    }
    
    /* Not available on NT4 -> disabled - we can live without this...
        SERVICE_DESCRIPTION sdBuf;
        sdBuf.lpDescription = this->descr;
        ChangeServiceConfig2(hService,SERVICE_CONFIG_DESCRIPTION, &sdBuf );
    */
    
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    Log(Notice) << "Service " << ServiceName << " has been installed." << LogEnd;
    return true;
}

bool TWinService::Uninstall()
{
    if (this->isRunning()) {
        Log(Crit) << "Unable to stop. Service " << ServiceName << " is running." << LogEnd;
        return false;
    }

    if (!this->IsInstalled()) {
        Log(Crit) << "Service " << ServiceName << " is not installed." << LogEnd;
        return false;
    }

    // Open the Service Control Manager
    SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (!hSCM) {
		Log(Crit) << "Unable to open Service Control Manager." << LogEnd;
		return false;
	}
    bool result = false;
    SC_HANDLE hService = ::OpenService(hSCM,ServiceName,DELETE);
    if (!hService) {
        Log(Crit) << "Unable to open " << ServiceName << " service for deletion." << LogEnd;
        CloseServiceHandle(hSCM);
        return false;
    }

    if (!DeleteService(hService)) {
        Log(Crit) << "Unable to delete " << ServiceName << " service." << LogEnd;
    	CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);
        return false;
	}

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    Log(Notice) << "Service " << ServiceName << " has been uninstalled." << LogEnd;
    return result;
}

/* this method is called when service is being started (by service itself) */
bool TWinService::RunService()
{
    SERVICE_TABLE_ENTRY st[] = {
        {ServiceName, ServiceMain},
        {NULL, NULL}
    };
    BOOL result = StartServiceCtrlDispatcher(st);
    return result?true:false;
}

/* this method is called from console, when someone wants to start service */
bool TWinService::StartService() {
    if (!IsInstalled()) {
        Log(Crit) << "Unable to start. Service " << ServiceName << " is not installed." << LogEnd;
        return false;
    }


    // open a handle to the SCM
	SC_HANDLE handle = ::OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if(!handle)	{
		Log(Crit) << "Could not connect to SCM dataase" << LogEnd;
		return false;
	}

	// open a handle to the service
	SC_HANDLE service = ::OpenService( handle, ServiceName, GENERIC_EXECUTE );
	if(!service) {
		::CloseServiceHandle( handle );
		Log(Crit) << "Could not get handle to " << ServiceName << " service" << LogEnd;
		return false;
	}

	// and start the service!
	if( !::StartService( service, 0, NULL ) ) {
		Log(Crit) << "Service " << ServiceName << " startup failed." << LogEnd;
    	::CloseServiceHandle( service );
	    ::CloseServiceHandle( handle );
        return false;
	}

    Log(Notice) << "Service " << ServiceName << " started." << LogEnd;
   	::CloseServiceHandle( service );
    ::CloseServiceHandle( handle );
    return true;
}

bool TWinService::StopService() 
{
    if (!IsInstalled()) {
        Log(Crit) << "Unable to stop. Service " << ServiceName << " is not installed." << LogEnd;
        return false;
    }

    // open a handle to the SCM
	SC_HANDLE handle = ::OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if(!handle)	{
		Log(Crit) << "Could not connect to SCM database" << LogEnd;
		return false;
	}

	// open a handle to the service
	SC_HANDLE service = ::OpenService( handle, ServiceName, GENERIC_EXECUTE );
	if(!service) {
		::CloseServiceHandle( handle );
        Log(Crit) << "Unable to open service " << ServiceName << LogEnd;
		return false;
	}

	// send the STOP control request to the service
	SERVICE_STATUS status;
	::ControlService( service, SERVICE_CONTROL_STOP, &status );
	::CloseServiceHandle( service );
	::CloseServiceHandle( handle );

    if (status.dwCurrentState == SERVICE_STOP_PENDING) {
        Log(Notice) << "Service " << ServiceName << " stop process initialized." << LogEnd;
        return true;
    }

	if( status.dwCurrentState != SERVICE_STOPPED ) {
		Log(Crit) << "Service " << ServiceName << " stop failed." << LogEnd;
        return false;
	}
	Log(Notice) << "Service " << ServiceName << " stopped." << LogEnd;
    return true;
}

void TWinService::SetStatus(DWORD dwState)
{
    Status.dwCurrentState = dwState;
    SetServiceStatus(hServiceStatus, &Status);
}

bool TWinService::Initialize()
{
    // Start the initialization
    SetStatus(SERVICE_START_PENDING);   
    // Perform the actual initialization
    bool result = OnInit(); 
    // Set final state
    Status.dwWin32ExitCode = GetLastError();
    Status.dwCheckPoint = 0;
    Status.dwWaitHint = 0;
    if (!result) {
        SetStatus(SERVICE_STOPPED);
        return false;    
    }
    SetStatus(SERVICE_RUNNING);
    return true;
}
void TWinService::Run()
{
	printf("WinService::Run()\n");
	return;
    while (IsRunning) {
        Sleep(1000);
    }
}

bool TWinService::OnInit()
{
	return true;
}

void TWinService::OnStop()
{
}

void TWinService::OnInterrogate()
{
}

void TWinService::OnPause()
{
}

void TWinService::OnContinue()
{
}

void TWinService::OnShutdown()
{
}

bool TWinService::OnUserControl(DWORD dwOpcode)
{
	return false;
}

int TWinService::getStatus() {
	return this->Status.dwCurrentState;
}

bool TWinService::isRunning() {
    return this->isRunning(this->ServiceName);
}

bool TWinService::isRunning(const char * name) {
    DWORD state = 0;

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if (!hSCM) {
    //Log(Crit) << "Unable to open Service Control Manager." << LogEnd;
	return false;
    }

    // Try to open the service
    SC_HANDLE hService = OpenService(hSCM,name, GENERIC_READ);
    if (!hService) {
	//Log(Crit) << "Unable to open " << name << " service." << LogEnd;
	CloseServiceHandle(hSCM);
	return false;
    }

    SERVICE_STATUS service;
    LPSERVICE_STATUS ptr = &service;
    memset((void*)&service,0, sizeof(SERVICE_STATUS) );
    if (QueryServiceStatus(hService, ptr)) {
	state = ptr->dwCurrentState;
    } 
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    
    switch (state) {
    case SERVICE_STOPPED:
        return false;
    case SERVICE_RUNNING:
        return true;
    case SERVICE_START_PENDING:
    default:
        return false;
    }
    return false;
}

void TWinService::showStatus() {
    bool serverInst, clientInst, relayInst;
    bool serverRun,  clientRun, relayRun;
    serverInst = this->IsInstalled("DHCPv6Server");
    clientInst = this->IsInstalled("DHCPv6Client");
    relayInst  = this->IsInstalled("DHCPv6Relay");
    relayRun   = this->isRunning("DHCPV6Relay");
    serverRun  = this->isRunning("DHCPv6Server");
    clientRun  = this->isRunning("DHCPv6Client");

    Log(Notice) <<  "Dibbler server : " << (serverInst? "INSTALLED":"NOT INSTALLED") 
                << ", " << (serverRun ? "RUNNING":"NOT RUNNING") << LogEnd;
    Log(Notice) <<  "Dibbler client : " << (clientInst? "INSTALLED":"NOT INSTALLED") 
                << ", " << (clientRun ? "RUNNING":"NOT RUNNING") << LogEnd;
    Log(Notice) <<  "Dibbler relay  : " << (relayInst ? "INSTALLED":"NOT INSTALLED") 
                << ", " << (relayRun  ? "RUNNING":"NOT RUNNING") << LogEnd;
}

bool TWinService::verifyPort() {
    // does this proper Dibbler port for this windows?	
    OSVERSIONINFO verinfo;
    verinfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    GetVersionEx(&verinfo);
    bool ok=false;
    if (verinfo.dwMajorVersion<5) { 
          Log(Notice) << "Windows NT detected (major=" << verinfo.dwMajorVersion
          << ", minor=" << verinfo.dwMinorVersion << "), so this is proper port." << LogEnd;
          ok = true;
    }
    if ((verinfo.dwMajorVersion==5) && (verinfo.dwMinorVersion==0)) {
         Log(Notice) << "Windows 2000 detected (major=" << verinfo.dwMajorVersion
          << ", minor=" << verinfo.dwMinorVersion << "), so this is proper port." << LogEnd;
          ok = true;
    }
    if (!ok)
    {
         Log(Warning) << "Unsupported operating system detected (major=" << verinfo.dwMajorVersion
          << ", minor=" << verinfo.dwMinorVersion << ")." << LogEnd;
         Log(Notice) << "Supported systems:" << LogEnd;
         Log(Notice) << "Windows NT4:   major<5 minor=0" << LogEnd;
         Log(Notice) << "Windows 2000:  major=5 minor=0" << LogEnd;
         Log(Notice) << "Unsupported systems (there's specific Dibbler version for recent systems):" << LogEnd;
         Log(Notice) << "Windows XP:    major=5 minor=1" << LogEnd;
         Log(Notice) << "Windows 2003:  major=5 minor=2" << LogEnd;
         Log(Notice) << "Windows Vista: major=6 minor=0" << LogEnd;
          
    }
    return ok;
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005-07-26 00:03:03  thomson
 * Preparation for relase 0.4.1
 *
 * Revision 1.2  2005/07/24 16:00:03  thomson
 * Port WinNT/2000 related changes.
 *
 * Revision 1.1  2005/07/23 14:33:22  thomson
 * Port for win2k/NT added.
 *
 */
