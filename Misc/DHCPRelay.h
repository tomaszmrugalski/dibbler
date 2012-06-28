/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <iostream>
#include <string>

class TDHCPRelay
{
  public:
    TDHCPRelay(const std::string& config);
    void run();
    void stop();
    bool isDone();
    bool checkPrivileges();
    void setWorkdir(std::string workdir);
    ~TDHCPRelay();

  private:
    bool IsDone;
};

#endif
