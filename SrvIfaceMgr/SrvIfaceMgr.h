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
