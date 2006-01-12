/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: CfgMgr.h,v 1.7 2006-01-12 00:23:34 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/01 00:57:36  thomson
 * no message
 *
 * Revision 1.5  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.4  2004/07/01 18:12:12  thomson
 * DUID creation failure results in client/server shutdown (bugs #44, #45)
 *
 * Revision 1.3  2004/05/23 21:35:31  thomson
 * *** empty log message ***
 *
 * Revision 1.2  2004/05/23 19:12:34  thomson
 * *** empty log message ***
 *
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

using namespace std;
class TCfgMgr
{
 public:
    TCfgMgr(SmartPtr<TIfaceMgr> IfaceMgr);
    virtual ~TCfgMgr();

    bool compareConfigs(const string cfgFile, const string oldCfgFile);
    void copyFile(const string cfgFile, const string oldCfgFile);
    SmartPtr<TDUID> getDUID();
    void setWorkdir(std::string workdir);
    int getLogLevel();
    string getWorkDir();
    string getLogName();
    
 protected:
    SmartPtr<TDUID> DUID;
    bool setDUID(const string duidFile);
    bool loadDUID(const string filename);
    bool generateDUID(const string duidFile,char * mac,int macLen, int macType);
    string Workdir;
    string LogName;
    int LogLevel;
    bool IsDone;
    SmartPtr<TIfaceMgr> IfaceMgr;
 private:
    
};
#endif
