#ifndef RELSERVICE_H
#define RELSERVICE_H

#include <windows.h>
#include <stdio.h>
#include "winservice.h"
#include "DHCPRelay.h"
class TRelService : public TWinService
{
public:
	TRelService(void);
	void Run();
    void OnStop();
    EServiceState ParseStandardArgs(int argc,char* argv[]);
	void setState(EServiceState status);
	~TRelService(void);
private:
	EServiceState status;
};

#endif