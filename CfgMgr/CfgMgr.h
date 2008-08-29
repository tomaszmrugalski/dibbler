/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: CfgMgr.h,v 1.10 2008-08-29 00:07:27 thomson Exp $
 *
 */

class TCfgMgr;
#ifndef CFGMGR_H
#define CFGMGR_H
#include <string>
#include "SmartPtr.h"
#include "DUID.h"
#include "IfaceMgr.h"

/* shared by server and relay */
#define RELAY_MIN_IFINDEX 1024

/* Defined DUID types */
using namespace std;

enum EDUIDType{
    DUID_TYPE_NOT_DEFINED = 0,
    DUID_TYPE_LLT = 1,
    DUID_TYPE_EN  = 2,
    DUID_TYPE_LL  = 3  /* not supported */
};

class TCfgMgr
{
 public:
    TCfgMgr(SPtr<TIfaceMgr> IfaceMgr);
    virtual ~TCfgMgr();

    bool compareConfigs(const string cfgFile, const string oldCfgFile);
    void copyFile(const string cfgFile, const string oldCfgFile);
    SPtr<TDUID> getDUID();
    void setWorkdir(std::string workdir);
    int getLogLevel();
    string getWorkDir();
    string getLogName();
    
 protected:
    SPtr<TDUID> DUID;
    bool setDUID(const string duidFile);
    bool loadDUID(const string filename);
    bool generateDUID(const string duidFile,char * mac,int macLen, int macType);
    string Workdir;
    string LogName;
    int LogLevel;
    bool IsDone;
    SPtr<TIfaceMgr> IfaceMgr;
    EDUIDType DUIDType;
    int DUIDEnterpriseNumber;
    SPtr<TDUID> DUIDEnterpriseID;
 private:
    
};
#endif
