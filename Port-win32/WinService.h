#ifndef _WINSERVICE_
#define _WINSERVICE_

#include <windows.h>
#include <string>

#define SERVICE_CONTROL_USER 128

typedef enum {
	STATUS,
	START,
	STOP,
	INSTALL,
	UNINSTALL,
	RUN,
	HELP,
	INVALID
} EServiceState;

class TWinService
{
public:
	std::string	ServiceDir;
	TWinService(const char* serviceName, const char* dispName,
				DWORD deviceType=SERVICE_DEMAND_START,
				char* dependencies=NULL, char* descr=NULL);
	static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI Handler(DWORD dwOpcode);
    void LogEvent(WORD wType, DWORD dwID, const char* pszS1 = NULL,
                  const char* pszS2 = NULL, const char* pszS3 = NULL);
    bool IsInstalled();
	bool IsInstalled(const char *name);
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
	void showStatus();

	int getStatus();
	bool isRunning(const char * name);

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
	char*                   descr;
	static	TWinService*	ServicePtr;
    HANDLE					EventSource;
};
#endif
