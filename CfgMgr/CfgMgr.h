/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: CfgMgr.h,v 1.2 2004-05-23 19:12:34 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

class TCfgMgr;
#ifndef CFGMGR_H
#define CFGMGR_H
#include <string>
#include "SmartPtr.h"
#include "DUID.h"
#include "IfaceMgr.h"

using namespace std;
class TCfgMgr
{
 public:
    TCfgMgr(SmartPtr<TIfaceMgr> IfaceMgr);
    virtual TCfgMgr::~TCfgMgr();

    bool compareConfigs(const string cfgFile, const string oldCfgFile);
    void copyFile(const string cfgFile, const string oldCfgFile);
    SmartPtr<TDUID>	getDUID();
    
 protected:
    SmartPtr<TDUID>	DUID;
    void setDUID(const string duidFile,char * mac,int macLen, int macType);
    bool TCfgMgr::loadDUID();
    void generateDUID(const string duidFile,char * mac,int macLen, int macType);
    string WorkDir;
    string LogName;
    int LogLevel;
    bool IsDone;
    SmartPtr<TIfaceMgr> IfaceMgr;
 private:
    
};
#endif
