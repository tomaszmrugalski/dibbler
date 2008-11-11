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

class TSrvIfaceMgr;
#ifndef SRVIFACEMGR_H
#define SRVIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"
#include "SrvTransMgr.h"
#include "SrvIfaceIface.h"
#include "SrvMsg.h"

class TSrvIfaceMgr :public TIfaceMgr {
 public:
    TSrvIfaceMgr(string xmlFile);
    ~TSrvIfaceMgr();
    friend ostream & operator <<(ostream & strum, TSrvIfaceMgr &x);

    SmartPtr<TSrvMsg> decodeMsg(SmartPtr<TSrvIfaceIface> ptrIface, 
				SmartPtr<TIPv6Addr> peer, 
				char * buf, int bufsize);
    
    SmartPtr<TSrvMsg> decodeRelayForw(SmartPtr<TSrvIfaceIface> ptrIface, 
				      SmartPtr<TIPv6Addr> peer, 
				      char * buf, int bufsize);
    
    bool setupRelay(string name, int ifindex, int underIfindex, SPtr<TSrvOptInterfaceID> interfaceID);
    void dump();
    
    // ---sends messages---
    bool send(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr, int port);
    
    // ---receives messages---
    SmartPtr<TSrvMsg> select(unsigned long timeout);

    // ---remember SmartPtrs to all menagers---
    void setContext(SPtr<TSrvIfaceMgr> srvIfaceMgr,
		    SPtr<TSrvTransMgr> srvTransMgr,
		    SPtr<TSrvCfgMgr> srvCfgMgr,
		    SPtr<TSrvAddrMgr> srvAddrMgr);

    void redetectIfaces();

  private:
    string XmlFile;
    SmartPtr<TSrvCfgMgr> SrvCfgMgr;
    SmartPtr<TSrvAddrMgr> SrvAddrMgr;
    SmartPtr<TSrvTransMgr> SrvTransMgr;
    SmartPtr<TSrvIfaceMgr> That;

};

#endif 
