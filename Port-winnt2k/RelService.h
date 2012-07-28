/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *          win2k version by <sob@hisoftware.cz>
 *
 * $Id: RelService.h,v 1.1 2005-07-23 14:33:22 thomson Exp $
 *                                                                           
 * Released under GNU GPL v2 licence                                
 *
 */

class TRelService;

#ifndef RELSERVICE_H
#define RELSERVICE_H

#include <windows.h>
#include <stdio.h>
#include "winservice.h"
#include "DHCPRelay.h"
extern TRelService StaticService;

class TRelService : public TWinService
{
public:
	TRelService(void);
	void Run();
    void OnStop();
    bool CheckAndInstall();
    EServiceState ParseStandardArgs(int argc,char* argv[]);
	void setState(EServiceState status);
    static TRelService * getHandle() { return &StaticService; }
	~TRelService(void);
private:
	EServiceState status;
};

#endif

/*
 * $Log: not supported by cvs2svn $
 */
