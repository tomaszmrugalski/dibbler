/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgReply.h,v 1.5 2008-08-29 00:07:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2006-10-06 00:43:28  thomson
 * Initial PD support.
 *
 * Revision 1.3  2005-01-08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

class TClntMsgReply;
#ifndef CLNTMSGREPLY_H
#define CLNTMSGREPLY_H
#include "ClntMsg.h"

class TClntMsgReply : public TClntMsg
{
  public:
    TClntMsgReply(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		  SmartPtr<TClntTransMgr> TransMgr, 
		  SmartPtr<TClntCfgMgr>   CfgMgr, 
		  SmartPtr<TClntAddrMgr> AddrMgr,
		  int iface, SmartPtr<TIPv6Addr> addr);

    TClntMsgReply(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		  SmartPtr<TClntTransMgr> TransMgr, 
		  SmartPtr<TClntCfgMgr>   CfgMgr, 
		  SmartPtr<TClntAddrMgr> AddrMgr,
		  int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);
	
    void answer(SmartPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgReply();
};

#endif /* CLNTMSGREPLY_H */
