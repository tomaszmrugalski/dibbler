/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Petr Pisar <petr.pisar(at)atlas(dot)cz>
 *
 * released under GNU GPL v2 only licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceMgr.h,v 1.11 2008-11-11 22:41:49 thomson Exp $
 *
 */

#ifndef SRVIFACEMGR_H
#define SRVIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "SrvIfaceIface.h"
#include "SrvMsg.h"

#define SrvIfaceMgr() (TSrvIfaceMgr::instance())

class TSrvIfaceMgr :public TIfaceMgr {
 public:
   static void instanceCreate(const std::string xmlDumpFile);
   static TSrvIfaceMgr &instance();

   ~TSrvIfaceMgr();
    friend ostream & operator <<(ostream & strum, TSrvIfaceMgr &x);

    SPtr<TSrvMsg> decodeMsg(SPtr<TSrvIfaceIface> ptrIface, 
				SPtr<TIPv6Addr> peer, 
				char * buf, int bufsize);
    
    SPtr<TSrvMsg> decodeRelayForw(SPtr<TSrvIfaceIface> ptrIface, 
				      SPtr<TIPv6Addr> peer, 
				      char * buf, int bufsize);
    
    bool setupRelay(string name, int ifindex, int underIfindex, SPtr<TSrvOptInterfaceID> interfaceID);
    void dump();
    
    // ---sends messages---
    bool send(int iface, char *msg, int size, SPtr<TIPv6Addr> addr, int port);
    
    // ---receives messages---
    SPtr<TSrvMsg> select(unsigned long timeout);

    void redetectIfaces();

  private:
    TSrvIfaceMgr(string xmlFile);
    static TSrvIfaceMgr * Instance;

    string XmlFile;
};

#endif 
