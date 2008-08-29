/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgRelease.h,v 1.5 2008-08-29 00:07:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2006-10-06 00:42:58  thomson
 * Initial PD support.
 *
 * Revision 1.3  2005-01-08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 *
 */

class TSrvMsgRelease;
#ifndef SRVMSGRELEASE_H
#define SRVMSGRELEASE_H

#include "SrvMsg.h"

class TSrvMsgRelease : public TSrvMsg
{
  public:
    TSrvMsgRelease(SmartPtr<TSrvIfaceMgr> IfMgr, 
		SmartPtr<TSrvTransMgr> TransMgr,
		SmartPtr<TSrvCfgMgr>   ConfMgr, 
		SmartPtr<TSrvAddrMgr>  AddrMgr, 
		 int iface, 
		 SmartPtr<TIPv6Addr> addr,
		 char* buf,
		 int bufSize);
	
    void doDuties();
    unsigned long getTimeout();
    bool check();
    string getName();
    ~TSrvMsgRelease();
};
#endif /* SRVMSGRELEASE_H */
