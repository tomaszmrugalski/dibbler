#ifndef SRVTSERVICE_H
#define SRVSERVICE_H

#include <windows.h>
#include <stdio.h>
#include "winservice.h"
#include "DHCPServer.h"
class TSrvService : public TWinService
{
public:
	TSrvService(void);
	void Run();
    void OnStop();
    EServiceState ParseStandardArgs(int argc,char* argv[]);
	void setState(EServiceState status);
	~TSrvService(void);
private:
	EServiceState status;
};

#endif