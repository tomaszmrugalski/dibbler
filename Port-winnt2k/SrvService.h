/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *          win2k version by <sob@hisoftware.cz>
 *
 * $Id: SrvService.h,v 1.1 2005-07-23 14:33:22 thomson Exp $
 *                                                                           
 * Released under GNU GPL v2 licence                                
 *
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
    bool CheckAndInstall();
    EServiceState ParseStandardArgs(int argc,char* argv[]);
	void setState(EServiceState status);
    static TSrvService * getHandle() { return &StaticService; }
	~TSrvService(void);
private:
	EServiceState status;
};

#endif

/*
 * $Log: not supported by cvs2svn $
 */
