/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgAdvertise.h,v 1.5 2008-08-29 00:07:34 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005-01-08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.3  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
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
    string getName();
    ~TSrvMsgAdvertise();
};

#endif /* SRVMSGADVERTISE_H */
