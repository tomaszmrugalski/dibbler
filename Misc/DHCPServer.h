#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <iostream>
#include <string>
#include "SmartPtr.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"

using namespace std;
class TDHCPServer
{
  public:
    TDHCPServer(string config);
    void run();
    void stop();
    bool isDone();
    bool checkPrivileges();
    ~TDHCPServer();

  private:
    bool IsDone;
    SmartPtr<TSrvIfaceMgr> IfaceMgr;
    SmartPtr<TSrvAddrMgr> AddrMgr;
    SmartPtr<TSrvCfgMgr> CfgMgr;
    SmartPtr<TSrvTransMgr> TransMgr;
};

#endif
