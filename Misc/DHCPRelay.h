/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DHCPRelay.h,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <iostream>
#include <string>
#include "RelCommon.h"

using namespace std;

class TDHCPRelay
{
  public:
    TDHCPRelay(string config);
    void run();
    void stop();
    bool isDone();
    bool checkPrivileges();
    ~TDHCPRelay();

  private:
    bool IsDone;
    TCtx Ctx;
};

#endif
