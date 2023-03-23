/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVINFREQUEST_H
#define SRVINFREQUEST_H

#include "IPv6Addr.h"
#include "SmartPtr.h"
#include "SrvMsg.h"

class TSrvMsgInfRequest : public TSrvMsg {
public:
  TSrvMsgInfRequest(int iface, SPtr<TIPv6Addr> addr, char *buf, int bufSize);
  void doDuties();
  bool check();
  unsigned long getTimeout();
  std::string getName() const;
  ~TSrvMsgInfRequest();

private:
  SPtr<TSrvAddrMgr> AddrMgr;
  List(TMsg) BackupSrvLst;
};

#endif /* SRVMSGINFREQUEST_H */
