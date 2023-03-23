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

#include "Portable.h"
#include "SmartPtr.h"
#include <iostream>
#include <string>

class TDHCPClient {
public:
  TDHCPClient(const std::string &config);
  void run();
  void stop();
  void resetLinkstate();
  bool isDone() const;
  bool checkPrivileges();
  void setWorkdir(const std::string &workdir);
#ifdef MOD_CLNT_CONFIRM
  void requestLinkstateChange();
#endif
  char *getCtrlIface();
  ~TDHCPClient();

private:
  void initLinkStateChange();
  bool IsDone_;
  volatile link_state_notify_t linkstates;
};

#endif
