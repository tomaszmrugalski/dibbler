/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceIface.h,v 1.1 2005-01-03 21:56:27 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

class TSrvIfaceIface;
#ifndef SRVIFACEIFACE_H
#define SRVIFACEIFACE_H

#include "Iface.h"

#define MAX_RELAYS 32

struct Relay {
    SmartPtr<TSrvIfaceIface> iface;
    int ifindex;
    int interfaceID;
};
typedef struct Relay TRelay;

class TSrvIfaceIface: public TIfaceIface {
 public:
    TSrvIfaceIface(const char * name, int id, unsigned int flags, char* mac, 
		   int maclen, char* llAddr, int llAddrCnt, int hwType);

    friend ostream & operator <<(ostream & strum, TSrvIfaceIface &x);

    void setUnderlaying(SmartPtr<TSrvIfaceIface> under);
    bool appendRelay(SmartPtr<TSrvIfaceIface> relay, int interfaceID);
    SmartPtr<TSrvIfaceIface> getUnderlaying();
    

 private:
    SmartPtr<TSrvIfaceIface> UnderRelay;
    bool Relay;

    TRelay Relays[MAX_RELAYS];
    int RelaysCnt;
};

#endif
