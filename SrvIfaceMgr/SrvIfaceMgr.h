/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceMgr.h,v 1.4 2005-01-03 21:56:09 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

class TSrvIfaceMgr;
#ifndef SRVIFACEMGR_H
#define SRVIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"
#include "SrvTransMgr.h"
#include "Msg.h"

/**
 * ServerInterfaceManager - sends and receives messages (server version)
 * @date 2003-10-14
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

class TSrvIfaceMgr :public TIfaceMgr {
 public:
    TSrvIfaceMgr(string xmlFile);
    ~TSrvIfaceMgr();
    friend ostream & operator <<(ostream & strum, TSrvIfaceMgr &x);

    bool setupRelay(string name, int ifindex, int underIfindex, int interfaceID);
    void dump();
    
    // ---sends messages---
    bool send(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr);
    
    // ---receives messages---
    SmartPtr<TMsg> select(unsigned long timeout);

    // ---remember SmartPtrs to all menagers---
    void setThats(SmartPtr<TSrvIfaceMgr> srvIfaceMgr,
		  SmartPtr<TSrvTransMgr> srvTransMgr,
		  SmartPtr<TSrvCfgMgr> srvCfgMgr,
		  SmartPtr<TSrvAddrMgr> srvAddrMgr);

  private:
    string XmlFile;
    SmartPtr<TSrvCfgMgr> SrvCfgMgr;
    SmartPtr<TSrvAddrMgr> SrvAddrMgr;
    SmartPtr<TSrvTransMgr> SrvTransMgr;
    SmartPtr<TSrvIfaceMgr> That;

};

#endif 
