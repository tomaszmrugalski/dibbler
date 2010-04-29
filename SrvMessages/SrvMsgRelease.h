/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 */

class TSrvMsgRelease;
#ifndef SRVMSGRELEASE_H
#define SRVMSGRELEASE_H

#include "SrvMsg.h"

class TSrvMsgRelease : public TSrvMsg
{
  public:
    TSrvMsgRelease(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize);
	
    void doDuties();
    unsigned long getTimeout();
    bool check();
    string getName();
    ~TSrvMsgRelease();
};
#endif /* SRVMSGRELEASE_H */
