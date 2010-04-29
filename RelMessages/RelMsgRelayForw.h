/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef RELMSGRELAYFORW_H
#define RELMSGRELAYFORW_H

#include "RelMsg.h"
#include "IPv6Addr.h"

#define MIN_RELAYFORW_LEN 34

class TRelMsgRelayForw: public TRelMsg {

 public:
    TRelMsgRelayForw(int iface, SPtr<TIPv6Addr> addr, char * data, int dataLen);
    string getName();
    bool check();

    int storeSelf(char * buffer);
    int getSize();

 private:
    SPtr<TIPv6Addr> PeerAddr;
    SPtr<TIPv6Addr> LinkAddr;
};

#endif
