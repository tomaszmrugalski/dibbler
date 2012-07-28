/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef SRVSERVICE_H
#define SRVSERVICE_H

#include <windows.h>
#include <stdio.h>
#include "winservice.h"
#include "DHCPServer.h"

class TSrvService;
extern TSrvService StaticService;

class TSrvService : public TWinService
{
public:
	TSrvService(void);
	void Run();
    void OnStop();
    EServiceState ParseStandardArgs(int argc,char* argv[]);
	void setState(EServiceState status);
    static TSrvService * getHandle() { return &StaticService; }
	~TSrvService(void);
private:
	EServiceState status;
};

#endif