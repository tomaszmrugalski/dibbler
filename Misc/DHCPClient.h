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

