/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvTransMgr.h,v 1.3 2004-09-03 20:58:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

class TSrvTransMgr;
#ifndef SRVTRANSMGR_H
#define SRVTRANSMGR_H

#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "Msg.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

class TSrvTransMgr
{
  public:
    TSrvTransMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr,
		 string newconf, string oldconf);

    long getTimeout();
    void relayMsg(SmartPtr<TMsg> msg);
    void doDuties();

    bool isDone();
    void shutdown();
    
	char * getCtrlAddr();
	int    getCtrlIface();
    
    void setThat(SmartPtr<TSrvTransMgr> that);
  private:
    TContainer< SmartPtr<TMsg> > MsgLst;
    bool IsDone;

    SmartPtr<TSrvIfaceMgr> IfaceMgr;
    SmartPtr<TSrvTransMgr> That;
    SmartPtr<TSrvCfgMgr>  CfgMgr;
    SmartPtr<TSrvAddrMgr>  AddrMgr;

	int ctrlIface;
	char ctrlAddr[48];

};



#endif


