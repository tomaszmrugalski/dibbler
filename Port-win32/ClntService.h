#ifndef _CLNTSERVICE_H
#define _CLNTSERVICE_H

#include <windows.h>
#include <stdio.h>
#include "winservice.h"
#include "DHCPClient.h"

class TClntService : public TWinService
{
public:
	TClntService(void);
	void Run();
    void OnStop();
    void OnShutdown();
	~TClntService(void);
    int ParseStandardArgs(int argc,char* argv[]);
private:
    string ServiceDir;
};

#endif