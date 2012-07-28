/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef RELSERVICE_H
#define RELSERVICE_H

#include <windows.h>
#include <stdio.h>
#include "winservice.h"
#include "DHCPRelay.h"
class TRelService;
extern TRelService StaticService;

class TRelService : public TWinService
{
public:
	TRelService(void);
	void Run();
    void OnStop();
    EServiceState ParseStandardArgs(int argc,char* argv[]);
	void setState(EServiceState status);
    static TRelService * getHandle() { return &StaticService; }
	~TRelService(void);
private:
	EServiceState status;
};

#endif