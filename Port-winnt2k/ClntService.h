/*                                                                           
 * Dibbler - a portable DHCPv6 
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *          win2k version by <sob@hisoftware.cz>
 *
 * $Id: ClntService.h,v 1.2 2005-07-24 16:00:03 thomson Exp $
 *                                                                           
 * Released under GNU GPL v2 licence                                
 *
 */

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
    bool CheckAndInstall();
	~TClntService(void);
    EServiceState ParseStandardArgs(int argc, char* argv[]);
	void setState(EServiceState status);
    static TClntService * getHandle() { return &StaticService; }

private:
	EServiceState status;
	
};

#endif
/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/07/23 14:33:22  thomson
 * Port for win2k/NT added.
 *
 */
 
 
