/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: DHCPClient.h,v 1.5 2008-08-17 22:41:43 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005-02-01 00:57:36  thomson
 * no message
 *
 * Revision 1.3  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.2  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 *                                                                           
 */

#ifndef DHCPCLIENT_H
#define DHCPCLIENT_H

#include <iostream>
#include <string>
#include "SmartPtr.h"
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
    void changeLinkstate();
    void resetLinkstate();
    bool isDone();
    bool checkPrivileges();
    void setWorkdir(std::string workdir);
    SmartPtr<TClntAddrMgr>  getAddrMgr();

    ~TDHCPClient();

  private:
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntAddrMgr>  AddrMgr;
    SmartPtr<TClntCfgMgr>   CfgMgr;
    SmartPtr<TClntTransMgr> TransMgr;
    bool IsDone;

};



#endif

