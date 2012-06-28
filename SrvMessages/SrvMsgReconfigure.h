/*
 * Dibbler - a portable DHCPv6
 *
 * author: Grzegorz Pluto
 * changes: Tomasz Mrugalski
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVMSGRECONFIGURE_H
#define SRVMSGRECONFIGURE_H

#include "SrvMsg.h"
#include "SrvMsgSolicit.h"
class TSrvMsgReconfigure : public TSrvMsg
{
  public:
    // creates object based on a buffer
    TSrvMsgReconfigure(int iface, SPtr<TIPv6Addr> addr, SPtr<TIPv6Addr> ia,
                       int msgType, SPtr<TDUID> ptrDUID);

    bool check();
    void doDuties();
    unsigned long getTimeout();
    std::string getName() const;
    ~TSrvMsgReconfigure();
};

#endif /* SRVMSGRECONFIGURE_H */
