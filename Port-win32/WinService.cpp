/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * Released under GNU GPL v2 licence
 *                                                                           
 * $Id: WinService.cpp,v 1.10 2004-12-02 00:51:06 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2004/09/28 21:49:32  thomson
 * no message
 *
 * Revision 1.8  2004/06/21 23:08:49  thomson
 * Minor fixes.
 *
 * Revision 1.7  2004/05/24 21:16:37  thomson
 * Various fixes.
 *
 * Revision 1.6  2004/04/15 23:53:45  thomson
 * Pathname installation fixed, run-time error checks disabled, winXP code cleanup.
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
    hServiceStatus = NULL;
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
    if (pService->hServiceStatus==NULL)
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
    bool result = false;
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = OpenService(hSCM,ServiceName,SERVICE_QUERY_CONFIG);
        if (hService) {
            result = true;
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCM);
    }
    return result;
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
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return false;
    // Get the executable file path
    char filePath[_MAX_PATH];
    GetModuleFileName(NULL, filePath, sizeof(filePath));
	int i = strlen(filePath); 
	sprintf(filePath+i, " start -d \"%s\"",ServiceDir.c_str());

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
        return FALSE;
    }

	SERVICE_DESCRIPTION sdBuf;
	sdBuf.lpDescription = this->descr;
	ChangeServiceConfig2(hService,SERVICE_CONFIG_DESCRIPTION, &sdBuf );

    CloseServiceHandle(hService);

    CloseServiceHandle(hSCM);
    return true;
}

bool TWinService::Uninstall()
{
   // Open the Service Control Manager
    SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (!hSCM) {
		printf("OpenSCManager() failed. Unable to open ServiceControl Manager\n");
		return false;
	}
    bool result = false;
    SC_HANDLE hService = ::OpenService(hSCM,ServiceName,DELETE);
    if (hService) 
	{
        if (DeleteService(hService)) 
		{
            result = true;
		} else {
			printf("DeleteService(hService) failed.\n");
		}
		CloseServiceHandle(hService);
	} else {
		printf("Unable to open %s service.\n",ServiceName);
	}
    CloseServiceHandle(hSCM);
    return result;
}

bool TWinService::StartService()
{
    SERVICE_TABLE_ENTRY st[] = {
        {ServiceName, ServiceMain},
        {NULL, NULL}
    };
    BOOL result = StartServiceCtrlDispatcher(st);
    return result?true:false;
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
        Sleep(5000);
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

bool TWinService::isRunning(const char * name) {
    bool result = false;
	bool ok;
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = OpenService(hSCM,name,SERVICE_QUERY_CONFIG);
        if (hService) {
			LPSERVICE_STATUS lpServiceStatus = 0;
			ok = ControlService(hService, SERVICE_CONTROL_INTERROGATE,lpServiceStatus);
			result = true;
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCM);
    }
    return result;
}

void TWinService::showStatus() {
    bool serverInstalled, clientInstalled;
    serverInstalled = this->IsInstalled("DHCPv6Server");
    clientInstalled = this->IsInstalled("DHCPv6Client");
    
    // FIXME
    Log(Notice) <<  "Dibbler server :" << (serverInstalled?"INSTALLED":"NOT INSTALLED") << LogEnd;
    Log(Notice) <<  "Dibbler client :" << (clientInstalled?"INSTALLED":"NOT INSTALLED") << LogEnd;
    Log(Notice) << "Client running: " << (this->isRunning("DHCPv6Client") ? "YES":"NO") << LogEnd;
}
