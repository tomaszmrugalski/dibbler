/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TSrvIfaceIface;
#ifndef SRVIFACEIFACE_H
#define SRVIFACEIFACE_H

#include "Iface.h"
#include "DHCPConst.h"
#include "SrvOptInterfaceID.h"

struct Relay {
    SPtr<TSrvIfaceIface> iface;
    int ifindex;
    SPtr<TSrvOptInterfaceID> interfaceID;
Relay() : ifindex(-1) {}
};
typedef struct Relay TRelay;

class TSrvIfaceIface: public TIfaceIface {
 public:
    TSrvIfaceIface(const char * name, int id, unsigned int flags, char* mac, 
		   int maclen, char* llAddr, int llAddrCnt, char * globalAddr, 
		   int globalAddrCnt, int hwType);

    friend std::ostream & operator <<(std::ostream & strum, TSrvIfaceIface &x);

    void setUnderlaying(SPtr<TSrvIfaceIface> under);
    bool appendRelay(SPtr<TSrvIfaceIface> relay, SPtr<TSrvOptInterfaceID> interfaceID);
    SPtr<TSrvIfaceIface> getUnderlaying();
    SPtr<TSrvIfaceIface> getRelayByInterfaceID(SPtr<TSrvOptInterfaceID> interfaceID);
    SPtr<TSrvIfaceIface> getRelayByLinkAddr(SPtr<TIPv6Addr> addr);
    SPtr<TSrvIfaceIface> getAnyRelay();
    int getRelayCnt();

 private:
    SPtr<TSrvIfaceIface> UnderRelay_;
    bool Relay_;

    TRelay Relays_[HOP_COUNT_LIMIT];
    int RelaysCnt_;
};

#endif
