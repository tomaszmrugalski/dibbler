/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVMSGREBIND_H
#define SRVMSGREBIND_H
#include "SrvMsg.h"

class TSrvMsgRebind : public TSrvMsg
{
  public:
    TSrvMsgRebind(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize);
    
    void doDuties();
    string getName();
    unsigned long getTimeout();
    bool check();
    ~TSrvMsgRebind();
};

#endif /* SRVMSGREBIND_H */
