/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelMsgRelayForw.h,v 1.3 2005-04-28 21:20:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/04/25 00:19:20  thomson
 * Changes in progress.
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

#ifndef RELMSGRELAYFORW_H
#define RELMSGRELAYFORW_H

#define MIN_RELAYFORW_LEN 34

class TRelMsgRelayForw: public TRelMsg {

 public:
    TRelMsgRelayForw(TCtx * ctx, int iface, SmartPtr<TIPv6Addr> addr, char * data, int dataLen);
    string getName();
    bool check();

    int storeSelf(char * buffer);
    int getSize();

 private:
    SmartPtr<TIPv6Addr> PeerAddr;
    SmartPtr<TIPv6Addr> LinkAddr;
};

#endif
