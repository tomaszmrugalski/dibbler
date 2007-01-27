/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Header: /var/cvs/dibbler/SrvIfaceMgr/SrvIfaceMgr.h,v 1.8 2007-01-27 17:14:11 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005-01-30 22:53:28  thomson
 * *** empty log message ***
 *
 * Revision 1.6  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.5  2005/01/03 23:13:57  thomson
 * Partial relay implementation.
 *
 * Revision 1.4  2005/01/03 21:56:09  thomson
 * Relay support added.
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

    SmartPtr<TSrvMsg> decodeMsg(SmartPtr<TSrvIfaceIface> ptrIface, 
				SmartPtr<TIPv6Addr> peer, 
				char * buf, int bufsize);
    
    SmartPtr<TSrvMsg> decodeRelayForw(SmartPtr<TSrvIfaceIface> ptrIface, 
				      SmartPtr<TIPv6Addr> peer, 
				      char * buf, int bufsize);
    
    bool setupRelay(string name, int ifindex, int underIfindex, int interfaceID);
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

  private:
    string XmlFile;
    SmartPtr<TSrvCfgMgr> SrvCfgMgr;
    SmartPtr<TSrvAddrMgr> SrvAddrMgr;
    SmartPtr<TSrvTransMgr> SrvTransMgr;
    SmartPtr<TSrvIfaceMgr> That;

};

#endif 
