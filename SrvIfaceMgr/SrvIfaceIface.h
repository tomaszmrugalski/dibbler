/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceIface.h,v 1.3 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/01/03 23:13:57  thomson
 * Partial relay implementation.
 *
 * Revision 1.1  2005/01/03 21:56:27  thomson
 * Initial version.
 *
 */

class TSrvIfaceIface;
#ifndef SRVIFACEIFACE_H
#define SRVIFACEIFACE_H

#include "Iface.h"
#include "DHCPConst.h"

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
    SmartPtr<TSrvIfaceIface> getRelayByInterfaceID(int interfaceID);
    int getRelayCnt();

 private:
    SmartPtr<TSrvIfaceIface> UnderRelay;
    bool Relay;

    TRelay Relays[HOP_COUNT_LIMIT];
    int RelaysCnt;
};

#endif
