/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: DHCPClient.h,v 1.2 2004-06-20 19:29:23 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

#ifndef DHCPCLIENT_H
#define DHCPCLIENT_H

#include <iostream>
#include <string>
#include "SmartPtr.h"
using namespace std;

class TClntIfaceMgr;
class TClntTransMgr;

class TDHCPClient
{
  public:
    TDHCPClient(string config);
    void run();
    void stop();
    bool isDone();
    bool checkPrivileges();

    ~TDHCPClient();

  private:
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntTransMgr> TransMgr;
    bool IsDone;

};



#endif

