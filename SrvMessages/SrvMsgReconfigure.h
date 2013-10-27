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
    TSrvMsgReconfigure(int iface, SPtr<TIPv6Addr> clientAddr,
                       int msgType, SPtr<TDUID> clientDuid);

    bool check();
    void doDuties();
    unsigned long getTimeout();
    std::string getName() const;
    ~TSrvMsgReconfigure();
};

#endif /* SRVMSGRECONFIGURE_H */
