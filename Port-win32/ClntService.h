#ifndef CLNTSERVICE_H
#define CLNTSERVICE_H

#include "winservice.h"

class TClntService;
extern TClntService StaticService;

class TClntService : public TWinService
{
public:
	TClntService(void);
	void Run();
    void OnStop();
    void OnShutdown();
	~TClntService(void);
    EServiceState ParseStandardArgs(int argc, char* argv[]);
	void setState(EServiceState status);
    static TClntService * getHandle() { return &StaticService; }

private:
	EServiceState status;
	
};

#endif
