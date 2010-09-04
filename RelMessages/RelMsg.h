/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TRelMsg;
#ifndef RELMSG_H
#define RELMSG_H

#include "Msg.h"

class TRelMsg : public TMsg
{
public:
    TRelMsg(int iface,  SPtr<TIPv6Addr> addr, char* data,  int dataLen);
    virtual bool check() = 0;
    void setDestination(int ifindex, SPtr<TIPv6Addr> dest);
    int getDestIface();
    SPtr<TIPv6Addr> getDestAddr();
    void decodeOpts(char * data, int dataLen);
    int getHopCount();
    
 protected:
    int DestIface;
    SPtr<TIPv6Addr>   DestAddr;

    int HopCount; // mormal messages =0, RELAY_FORW, RELAY_REPL = (0..32)
};

#endif
