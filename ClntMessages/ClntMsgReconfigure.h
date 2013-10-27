/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Grzegorz Pluto
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#ifndef CLNTMSGRECONFIGURE_H
#define CLNTMSGRECONFIGURE_H

#include "ClntMsg.h"

class TClntMsgReconfigure : public TClntMsg
{
  public:
    TClntMsgReconfigure(int iface, SPtr<TIPv6Addr> addr,
                      char* buf, int bufSize);

    bool check();
    void doDuties();
    std::string getName() const { return std::string("RECONFIGURE"); }
    ~TClntMsgReconfigure();
};

#endif
