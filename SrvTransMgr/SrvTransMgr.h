/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvTransMgr.h,v 1.10 2007-01-27 17:13:44 thomson Exp $
 *
 */

class TSrvTransMgr;
#ifndef SRVTRANSMGR_H
#define SRVTRANSMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "SrvMsg.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

class TSrvTransMgr
{
    friend ostream & operator<<(ostream &strum, TSrvTransMgr &x);
  public:
    TSrvTransMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvCfgMgr> cfgMgr,
		 string xmlFile);
    ~TSrvTransMgr();

    bool openSocket(SmartPtr<TSrvCfgIface> confIface);

    long getTimeout();
    void relayMsg(SmartPtr<TSrvMsg> msg);
    void doDuties();
    void dump();

    bool isDone();
    void shutdown();
    
    char * getCtrlAddr();
    int    getCtrlIface();
    
    void setContext(SmartPtr<TSrvTransMgr> transMgr);
  private:
    string XmlFile;
    List(TSrvMsg) MsgLst;
    bool IsDone;

    SmartPtr<TSrvIfaceMgr> IfaceMgr;
    SmartPtr<TSrvTransMgr> That;
    SmartPtr<TSrvCfgMgr>  CfgMgr;
    SmartPtr<TSrvAddrMgr>  AddrMgr;

    int ctrlIface;
    char ctrlAddr[48];
};



#endif


