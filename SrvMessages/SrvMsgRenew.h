/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * chamges Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVMSGRENEW_H
#define SRVMSGRENEW_H
#include "SrvMsg.h"

class TSrvMsgRenew : public TSrvMsg
{
 public:
    TSrvMsgRenew(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize);
    
    void doDuties();
    unsigned long getTimeout();
    string getName();
    bool check();
    ~TSrvMsgRenew();
};

#endif /* SRVMSGRENEW_H */
