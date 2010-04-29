/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TRelMsgRelayRepl;
#ifndef RELMSGRELAYREPL_H
#define RELMSGRELAYREPL_H

#include "RelMsg.h"

#define MIN_RELAYREPL_LEN 34

class TRelMsgRelayRepl: public TRelMsg {
 public:
    TRelMsgRelayRepl(int iface, SPtr<TIPv6Addr> addr, char * data, int dataLen);
    int getSize();
    int storeSelf(char * buffer);
    string getName();
    bool check();

 private:
    SPtr<TIPv6Addr> PeerAddr;
    SPtr<TIPv6Addr> LinkAddr;
};

#endif
