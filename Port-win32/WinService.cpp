#include <windows.h>
#include <stdio.h>
#include "winservice.h"

TWinService* TWinService::ServicePtr= NULL;

TWinService::TWinService(const char* serviceName, const char* dispName, 
			 DWORD serviceType, char* dependencies)
{
    ServicePtr= this;
	
    strncpy(ServiceName, serviceName, sizeof(ServiceName)-1);
    ServiceType=serviceType;
    Dependencies=dependencies;
    MajorVersion = 1;
    MinorVersion = 0;
    EventSource = NULL;
    
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
    DebugMsg("TWinService::~TWinService()");
    if (EventSource)
        DeregisterEventSource(EventSource);
}

void TWinService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Get a pointer to the C++ object
	//DebugBreak();
    TWinService* pService = ServicePtr;
    
    //FIXME:pService->DebugMsg("Entering CNTService::ServiceMain()");
    // Register the control request handler
    pService->Status.dwCurrentState = SERVICE_START_PENDING;
    pService->hServiceStatus = RegisterServiceCtrlHandler(pService->ServiceName,Handler);
    if (pService->hServiceStatus==NULL)
	{
        //FIXME:pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_CTRLHANDLERNOTINSTALLED);
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
    //FIXME:pService->DebugMsg("Leaving CNTService::ServiceMain()");
}
void TWinService::Handler(DWORD dwOpcode)
{
	//DebugBreak();
    // Get a pointer to the object
	TWinService* pService = ServicePtr;
    
    //FIXME:pService->DebugMsg("CNTService::Handler(%lu)", dwOpcode);
    switch (dwOpcode) {
    case SERVICE_CONTROL_STOP: // 1
        pService->SetStatus(SERVICE_STOP_PENDING);
        pService->OnStop();
        pService->IsRunning = FALSE;
        //FIXME:pService->LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
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
                //FIXME:pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
            }
        } 
		else 
		{
            //FIXME:pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
        }
        break;
    }

    // Report current status
    //FIXME:pService->DebugMsg("Updating status (%lu, %lu)",
	//                       pService->m_hServiceStatus,
	//                       pService->m_Status.dwCurrentState);
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

bool TWinService::ParseStandardArgs(int argc, char* argv[])
{
    // See if we have any command line args we recognise
    if (argc <= 1) return false;

    if (_stricmp(argv[1], "-v") == 0) {
	//Display version
        printf("%s Wersja %d.%d\n",
               ServiceName, MajorVersion, MinorVersion);
        printf("Service is %s installed.\n",
			IsInstalled() ? " " : "NOT");
        return true;
    } 
    if (_stricmp(argv[1], "-i") == 0) 
    {
	// Install service
	if (IsInstalled())
	    printf("%s is already installed\n", ServiceName);
	else
	    if (Install())
		printf("Usluga '%s' jest zainstslowana\n", ServiceName);
	    else
		printf("Nie mozna zainstalowac uslugi '%s' Blad: %d\n", 
		       ServiceName, GetLastError());
	return true; 
    } 
    if (_stricmp(argv[1], "-u") == 0) 
    {
	// Uninstall service
	if (!IsInstalled()) 
	    printf("Usluga %s nie jest zainstalowana\n", ServiceName);
	else 
	    if (Uninstall()) 
	    {
		// Get the executable file path
		char filePath[_MAX_PATH];
		GetModuleFileName(NULL, filePath, sizeof(filePath));
		printf("Usluga %s zostala usunnieta. (Mozesz bezpiecznie usunac plik (%s).)\n",
		       ServiceName, filePath);
            } else
                printf("Nie mozna usunac uslugi '%s'. Blad %d\n", ServiceName, GetLastError());
	return true; 
    }
    return false;
}

bool TWinService::IsInstalled()
{
    bool result = false;
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM) 
    {
        // Try to open the service
        SC_HANDLE hService = OpenService(hSCM,ServiceName,SERVICE_QUERY_CONFIG);
        if (hService) 
	{
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
	sprintf(filePath+i, " -d \"%s\"",ServiceDir.c_str());

    // Create the service
	printf("Install(): filepath=[%s] ServiceName=[%s]\n",filePath,ServiceName);
	printf("ServiceDir=[%s]\n",ServiceDir.c_str());
    SC_HANDLE hService = CreateService(	hSCM,ServiceName,ServiceName,
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

	//ChangeServiceConfig2(hService,SERVICE_CONFIG_DESCRIPTION,"To jest opis uslugi" );

    // make registry entries to support logging messages
    // add the source name as a subkey under the Application
    // key in the EventLog service portion of the registry.
    char key[256];
    HKEY hKey = NULL;
    strcpy(key, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
    strcat(key, ServiceName);
    if (RegCreateKey(HKEY_LOCAL_MACHINE, key, &hKey) != ERROR_SUCCESS) {
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);
        return FALSE;
    }

    // Add the Event ID message-file name to the 'EventMessageFile' subkey.
    RegSetValueEx(hKey,
                  "EventMessageFile",
                  0,
                  REG_EXPAND_SZ, 
                  (CONST BYTE*)filePath,
                  (DWORD)strlen(filePath) + 1);

    // Set the supported types flags.
    DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    RegSetValueEx(hKey,
                  "TypesSupported",
                  0,
                  REG_DWORD,
                  (CONST BYTE*)&dwData,
                  sizeof(DWORD));
    RegCloseKey(hKey);

//    FIXME:LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, ServiceName);

    // tidy up
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
    //FIXME:DebugMsg("Calling StartServiceCtrlDispatcher()");
    BOOL result = StartServiceCtrlDispatcher(st);
    //FIXME:DebugMsg("Returned from StartServiceCtrlDispatcher()");
	return result?true:false;
}

void TWinService::SetStatus(DWORD dwState)
{
    //FIXME:DebugMsg("CNTService::SetStatus(%lu, %lu)", m_hServiceStatus, dwState);
    Status.dwCurrentState = dwState;
    SetServiceStatus(hServiceStatus, &Status);
}

bool TWinService::Initialize()
{
    //FIXME:DebugMsg("Entering CNTService::Initialize()");
    // Start the initialization
    SetStatus(SERVICE_START_PENDING);   
    // Perform the actual initialization
    bool result = OnInit(); 
    // Set final state
    Status.dwWin32ExitCode = GetLastError();
    Status.dwCheckPoint = 0;
    Status.dwWaitHint = 0;
    if (!result) {
        //FIXME:LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILEDINIT);
        SetStatus(SERVICE_STOPPED);
        return false;    
    }
    //FIXME:LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED);
    SetStatus(SERVICE_RUNNING);
    //FIXME:DebugMsg("Leaving CNTService::Initialize()");
    return true;
}
void TWinService::Run()
{
    //FIXME:DebugMsg("Entering CNTService::Run()");

    while (IsRunning) {
        DebugMsg("Sleeping...");
        Sleep(5000);
    }

    // nothing more to do
    //FIXME:DebugMsg("Leaving CNTService::Run()");
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

void TWinService::DebugMsg(const char* Format, ...)
{
    char buf[1024];
    sprintf(buf, "[%s](%lu): ", ServiceName, GetCurrentThreadId());
	va_list arglist;
	va_start(arglist, Format);
    vsprintf(&buf[strlen(buf)], Format, arglist);
	va_end(arglist);
    strcat(buf, "\n");
    OutputDebugString(buf);
}
