/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgReply.h,v 1.7 2006-08-21 21:33:21 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2006-07-03 20:15:57  thomson
 * FQDN support partially supported (still does not work properly).
 *
 * Revision 1.5  2006/02/02 23:18:30  thomson
 * 0.4.2 release.
 *
 * Revision 1.4  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.3  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
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

#include "SrvOptClientIdentifier.h"

class TSrvMsgReply : public TSrvMsg
{
  public:
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
