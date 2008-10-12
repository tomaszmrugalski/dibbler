/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: DHCPClient.h,v 1.7 2008-10-12 14:07:31 thomson Exp $
 *                                                                           
 */

#ifndef DHCPCLIENT_H
#define DHCPCLIENT_H

#include <iostream>
#include <string>
#include "SmartPtr.h"
#include "Portable.h"

using namespace std;

class TClntIfaceMgr;
class TClntAddrMgr;
class TClntCfgMgr;
class TClntTransMgr;

class TDHCPClient
{
  public:
    TDHCPClient(string config);
    void run();
    void stop();
    void resetLinkstate();
    bool isDone();
    bool checkPrivileges();
    void setWorkdir(std::string workdir);
    SmartPtr<TClntAddrMgr>  getAddrMgr();
    SmartPtr<TClntCfgMgr> getCfgMgr();
    char* getCtrlIface();
    ~TDHCPClient();

  private:
    void initLinkStateChange();

    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntAddrMgr>  AddrMgr;
    SmartPtr<TClntCfgMgr>   CfgMgr;
    SmartPtr<TClntTransMgr> TransMgr;
    bool IsDone;
    volatile link_state_notify_t linkstates;
};

#endif

