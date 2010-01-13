/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceIface.h,v 1.8 2008-11-11 22:41:49 thomson Exp $
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
};
typedef struct Relay TRelay;

class TSrvIfaceIface: public TIfaceIface {
 public:
    TSrvIfaceIface(const char * name, int id, unsigned int flags, char* mac, 
		   int maclen, char* llAddr, int llAddrCnt, char * globalAddr, 
		   int globalAddrCnt, int hwType);

    friend ostream & operator <<(ostream & strum, TSrvIfaceIface &x);

    void setUnderlaying(SPtr<TSrvIfaceIface> under);
    bool appendRelay(SPtr<TSrvIfaceIface> relay, SPtr<TSrvOptInterfaceID> interfaceID);
    SPtr<TSrvIfaceIface> getUnderlaying();
    SPtr<TSrvIfaceIface> getRelayByInterfaceID(SPtr<TSrvOptInterfaceID> interfaceID);
    SPtr<TSrvIfaceIface> getRelayByLinkAddr(SPtr<TIPv6Addr> addr);
    SPtr<TSrvIfaceIface> getAnyRelay();
    int getRelayCnt();

 private:
    SPtr<TSrvIfaceIface> UnderRelay;
    bool Relay;

    TRelay Relays[HOP_COUNT_LIMIT];
    int RelaysCnt;
};

#endif
