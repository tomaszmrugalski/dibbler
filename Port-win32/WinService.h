#ifndef _WINSERVICE_
#define _WINSERVICE_

#include <windows.h>
#include <string>

//#include "ntservmsg.h" // Event message ids
#define SERVICE_CONTROL_USER 128


class TWinService
{
public:
	TWinService(const char* serviceName, const char* dispName,
					DWORD deviceType=SERVICE_DEMAND_START,
						char* dependencies=NULL);
	static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI Handler(DWORD dwOpcode);
    void LogEvent(WORD wType, DWORD dwID,
		          const char* pszS1 = NULL,
                  const char* pszS2 = NULL,
                  const char* pszS3 = NULL);
    bool ParseStandardArgs(int argc, char* argv[]);
    bool IsInstalled();
    bool Install();
    bool Uninstall();
    bool StartService();
    void SetStatus(DWORD dwState);
    bool Initialize();
    
	virtual void Run();
	virtual bool OnInit();
    virtual void OnStop();
    virtual void OnInterrogate();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
    virtual bool OnUserControl(DWORD dwOpcode);
    void DebugMsg(const char* Format, ...);

	int getStatus();
	bool isRunning();

	~TWinService(void);

protected:
    SERVICE_STATUS			Status;
	SERVICE_STATUS_HANDLE	hServiceStatus;
    BOOL					IsRunning;
	
	char					ServiceName[64];
    int						MajorVersion;
    int						MinorVersion;
	DWORD					ServiceType;
	char*					Dependencies;
	char*					DisplayName;
	//Trick to enable static functions to call methods,
	//drawback - but there can be only one instance of this class
	static	TWinService*	ServicePtr;
    HANDLE					EventSource;

	std::string				ServiceDir;
};
#endif