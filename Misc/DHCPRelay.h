/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DHCPRelay.h,v 1.2 2005-02-01 00:57:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 */

#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <iostream>
#include <string>

using namespace std;

class TDHCPRelay
{
  public:
    TDHCPRelay(string config);
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
