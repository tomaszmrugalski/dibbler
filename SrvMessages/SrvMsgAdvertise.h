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
class TSrvMsgAdvertise : public TSrvMsg
{
  public:
    // creates object based on a buffer
    TSrvMsgAdvertise(SPtr<TSrvMsg> question);

    bool check();
    bool handleSolicitOptions(SPtr<TSrvMsg> solicit);
    void doDuties();
    unsigned long getTimeout();
    std::string getName() const;
    ~TSrvMsgAdvertise();
};

#endif /* SRVMSGADVERTISE_H */
