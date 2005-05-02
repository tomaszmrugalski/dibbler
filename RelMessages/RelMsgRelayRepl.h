/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelMsgRelayRepl.h,v 1.3 2005-05-02 20:58:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/04/28 21:20:52  thomson
 * Support for multiple relays added.
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

class TRelMsgRelayRepl;
#ifndef RELMSGRELAYREPL_H
#define RELMSGRELAYREPL_H

#include "RelMsg.h"
#include "RelCommon.h"

#define MIN_RELAYREPL_LEN 34

class TRelMsgRelayRepl: public TRelMsg {
 public:
    TRelMsgRelayRepl(TCtx * ctx, int iface, SmartPtr<TIPv6Addr> addr, char * data, int dataLen);
    int getSize();
    int storeSelf(char * buffer);
    string getName();
    bool check();

 private:
    SmartPtr<TIPv6Addr> PeerAddr;
    SmartPtr<TIPv6Addr> LinkAddr;
};

#endif
