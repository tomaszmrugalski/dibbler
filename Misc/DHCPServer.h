/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <iostream>
#include <string>
#include "SmartPtr.h"

class TDHCPServer
{
  public:
    TDHCPServer(const std::string& config);
    void run();
    void stop();
    bool isDone();
    bool checkPrivileges();
    void setWorkdir(std::string workdir);
    ~TDHCPServer();

  private:
    bool IsDone_;
};

#endif
