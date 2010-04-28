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

class TDHCPClient
{
  public:
    TDHCPClient(const std::string config);
    void run();
    void stop();
    void resetLinkstate();
    bool isDone();
    bool checkPrivileges();
    void setWorkdir(const std::string workdir);
#ifdef MOD_CLNT_CONFIRM
    void requestLinkstateChange();
#endif
    char* getCtrlIface();
    ~TDHCPClient();

  private:
    void initLinkStateChange();
    bool IsDone;
    volatile link_state_notify_t linkstates;
};

#endif

