/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgAdvertise.h,v 1.3 2004-09-05 15:27:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 *
 */

#ifndef SRVMSGADVERTISE_H
#define SRVMSGADVERTISE_H

#include "SrvMsg.h"
#include "SrvMsgSolicit.h"
class TSrvMsgAdvertise : public TSrvMsg
{
  public:
    // creates object based on a buffer
    TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		     SmartPtr<TSrvTransMgr> TransMgr,
		     SmartPtr<TSrvCfgMgr> CfgMgr,
		     SmartPtr<TSrvAddrMgr> AddrMgr,
		     int iface, SmartPtr<TIPv6Addr> addr);
    
    TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		     SmartPtr<TSrvTransMgr> TransMgr,
		     SmartPtr<TSrvCfgMgr> CfgMgr,
		     SmartPtr<TSrvAddrMgr> AddrMgr,
		     SmartPtr<TSrvMsgSolicit> question);
    
	TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		SmartPtr<TSrvTransMgr> TransMgr,
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		unsigned int iface, SmartPtr<TIPv6Addr> addr,
		unsigned char* buf, unsigned int bufSize);

    bool check();
    bool answer(SmartPtr<TSrvMsgSolicit> solicit);
    void answer(SmartPtr<TMsg> Rep);
    void doDuties();
    unsigned long getTimeout();
    string getName();
    ~TSrvMsgAdvertise();
};

#endif /* SRVMSGADVERTISE_H */
