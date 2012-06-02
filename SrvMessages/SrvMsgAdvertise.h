/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVMSGADVERTISE_H
#define SRVMSGADVERTISE_H

#include "SrvMsg.h"
#include "SrvMsgSolicit.h"
class TSrvMsgAdvertise : public TSrvMsg
{
  public:
    // creates object based on a buffer
    TSrvMsgAdvertise(int iface, SPtr<TIPv6Addr> addr);
    TSrvMsgAdvertise(SPtr<TSrvMsgSolicit> question);
    TSrvMsgAdvertise(unsigned int iface, SPtr<TIPv6Addr> addr,unsigned char* buf, unsigned int bufSize);
    /// @todo: get rid of 2 of those constructors

    bool check();
    bool handleSolicitOptions(SPtr<TSrvMsgSolicit> solicit);
    void doDuties();
    unsigned long getTimeout();
    std::string getName() const;
    ~TSrvMsgAdvertise();
};

#endif /* SRVMSGADVERTISE_H */
