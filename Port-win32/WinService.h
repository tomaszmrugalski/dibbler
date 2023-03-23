/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Hernan Martinez <hernan(dot)c(dot)martinez(at)gmail(dot)com>
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef _WINSERVICE_
#define _WINSERVICE_

#include <string>
#include <windows.h>

#define SERVICE_CONTROL_USER 128

typedef enum {
  STATUS,
  START,
  STOP,
  INSTALL,
  UNINSTALL,
  SERVICE,
  RUN,
  HELP,
  INVALID
} EServiceState;

class TWinService {
public:
  std::string ServiceDir;
  TWinService(const char *serviceName, const char *dispName,
              DWORD deviceType = SERVICE_DEMAND_START,
              char *dependencies = NULL, char *descr = NULL);
  static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
  static void WINAPI Handler(DWORD dwOpcode);
  void LogEvent(WORD wType, DWORD dwID, const char *pszS1 = NULL,
                const char *pszS2 = NULL, const char *pszS3 = NULL);
  bool IsInstalled();
  bool IsInstalled(const char *name);
  bool Install();
  bool Uninstall();
  bool StartService(); /* invoked to trigger start */
  bool RunService();   /* actual service start and run */
  bool StopService();
  void SetStatus(DWORD dwState);
  bool Initialize();
  void showStatus();
  bool verifyPort();
  int getStatus();
  bool isRunning(const char *name);
  bool isRunning();
  bool isRunAsAdmin();

  virtual void Run();
  virtual bool OnInit();
  virtual void OnStop();
  virtual void OnInterrogate();
  virtual void OnPause();
  virtual void OnContinue();
  virtual void OnShutdown();
  virtual bool OnUserControl(DWORD dwOpcode);

  ~TWinService(void);

  static const char ADMIN_REQUIRED_STR[];

protected:
  SERVICE_STATUS Status;
  SERVICE_STATUS_HANDLE hServiceStatus;
  BOOL IsRunning;

  char ServiceName[64];
  int MajorVersion;
  int MinorVersion;
  DWORD ServiceType;
  char Dependencies[64];
  char *DisplayName;
  char *descr;
  static TWinService *ServicePtr;
  HANDLE EventSource;
};
#endif
