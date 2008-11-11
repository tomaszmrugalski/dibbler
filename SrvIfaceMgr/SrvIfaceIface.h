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
    SmartPtr<TSrvIfaceIface> iface;
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

    void setUnderlaying(SmartPtr<TSrvIfaceIface> under);
    bool appendRelay(SmartPtr<TSrvIfaceIface> relay, SPtr<TSrvOptInterfaceID> interfaceID);
    SmartPtr<TSrvIfaceIface> getUnderlaying();
    SmartPtr<TSrvIfaceIface> getRelayByInterfaceID(SPtr<TSrvOptInterfaceID> interfaceID);
    SmartPtr<TSrvIfaceIface> getRelayByLinkAddr(SmartPtr<TIPv6Addr> addr);
    SmartPtr<TSrvIfaceIface> getAnyRelay();
    int getRelayCnt();

 private:
    SmartPtr<TSrvIfaceIface> UnderRelay;
    bool Relay;

    TRelay Relays[HOP_COUNT_LIMIT];
    int RelaysCnt;
};

#endif
