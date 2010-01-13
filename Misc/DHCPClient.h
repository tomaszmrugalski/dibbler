/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: DHCPClient.h,v 1.8 2009-03-24 23:17:18 thomson Exp $
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
#ifdef MOD_CLNT_CONFIRM
    void requestLinkstateChange();
#endif
    SPtr<TClntAddrMgr>  getAddrMgr();
    SPtr<TClntCfgMgr> getCfgMgr();
    char* getCtrlIface();
    ~TDHCPClient();

  private:
    void initLinkStateChange();

    SPtr<TClntIfaceMgr> IfaceMgr;
    SPtr<TClntAddrMgr>  AddrMgr;
    SPtr<TClntCfgMgr>   CfgMgr;
    SPtr<TClntTransMgr> TransMgr;
    bool IsDone;
    volatile link_state_notify_t linkstates;
};

#endif

