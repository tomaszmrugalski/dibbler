/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvTransMgr.h,v 1.12 2008-10-12 20:10:25 thomson Exp $
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
    TSrvTransMgr(SPtr<TSrvIfaceMgr> ifaceMgr,
		 SPtr<TSrvAddrMgr> addrMgr,
		 SPtr<TSrvCfgMgr> cfgMgr,
		 string xmlFile);
    ~TSrvTransMgr();

    bool openSocket(SPtr<TSrvCfgIface> confIface);

    long getTimeout();
    void relayMsg(SPtr<TSrvMsg> msg);
    void doDuties();
    void dump();

    bool isDone();
    void shutdown();

    char * getCtrlAddr();
    int    getCtrlIface();

    void setContext(SPtr<TSrvTransMgr> transMgr);

    SPtr<TSrvMsg> requestMsg;
  private:
    string XmlFile;
    List(TSrvMsg) MsgLst;
    bool IsDone;

    SPtr<TSrvIfaceMgr> IfaceMgr;
    SPtr<TSrvTransMgr> That;
    SPtr<TSrvCfgMgr>  CfgMgr;
    SPtr<TSrvAddrMgr>  AddrMgr;

    int ctrlIface;
    char ctrlAddr[48];
};



#endif


