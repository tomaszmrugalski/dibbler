/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgDecline.h,v 1.2 2004-06-20 17:25:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

class TSrvMsgDecline;
#ifndef SRVMSGDECLINE_H
#define SRVMSGDECLINE_H
#include "SrvMsg.h"

class TSrvMsgDecline : public TSrvMsg
{
  public:
    TSrvMsgDecline(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr> CfgMgr,
		   SmartPtr<TSrvAddrMgr> AddrMgr,
		   int iface,  SmartPtr<TIPv6Addr> addr);
    
    TSrvMsgDecline(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr> CfgMgr,
		   SmartPtr<TSrvAddrMgr> AddrMgr,
		   int iface, SmartPtr<TIPv6Addr> addr,
		   char* buf, int bufSize);
    
    bool  check();
    string getName();
    void  answer(SmartPtr<TMsg> Rep);
    void  doDuties();
    unsigned long  getTimeout();
    ~TSrvMsgDecline();
};

#endif /* SRVMSGDECLINE_H */
