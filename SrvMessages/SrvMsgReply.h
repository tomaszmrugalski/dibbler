/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgReply.h,v 1.2 2004-06-20 17:25:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

class TSrvMsgReply;
#ifndef SRVMSGREPLY_H
#define SRVMSGREPLY_H
#include "SrvMsg.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgDecline.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRelease.h"
#include "SrvMsgSolicit.h"
#include "SrvMsgInfRequest.h"

class TSrvMsgReply : public TSrvMsg
{
  public:
/*  Server does not receive REPLY
    TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
		 SmartPtr<TSrvTransMgr>, 
		 SmartPtr<TSrvCfgMgr>, 
		 SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface,  char* addr);
    
    TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
		 SmartPtr<TSrvTransMgr>, 
		 SmartPtr<TSrvCfgMgr>, 
		 SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, 
		 char* addr,
		 char* buf,
		 int bufSize); */
    
	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
		     SmartPtr<TSrvTransMgr>, 
		     SmartPtr<TSrvCfgMgr>, 
		     SmartPtr<TSrvAddrMgr> AddrMgr,
		     SmartPtr<TSrvMsgConfirm> question);
	
	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgDecline> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRebind> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRelease> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRenew> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRequest> question);

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgSolicit> question);

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
          SmartPtr<TSrvMsgInfRequest> question);

    void answer(SmartPtr<TMsg> Rep);
    void doDuties();
    unsigned long getTimeout();
    bool check();
    string getName();
    
    ~TSrvMsgReply();
private:
    SmartPtr<TSrvOptOptionRequest> reqOpts;
    SmartPtr<TSrvOptClientIdentifier> duidOpt;

    void appendDefaultOption(SmartPtr<TOpt> ptrOpt);

    void setOptionsReqOptClntDUID(SmartPtr<TMsg> msg);


};


#endif /* SRVMSGREPLY_H */
