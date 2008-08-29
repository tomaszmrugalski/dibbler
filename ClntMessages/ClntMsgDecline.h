/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgDecline.h,v 1.4 2008-08-29 00:07:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005-01-08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

class TClntMsgDecline;
#ifndef CLNTMSGDECLINE_H
#define CLNTMSGDECLINE_H
#include "ClntMsg.h"

class TClntMsgDecline : public TClntMsg
{
  public:
    TClntMsgDecline(SmartPtr<TClntIfaceMgr> IfaceMgr,
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr> CfgMgr,
		    SmartPtr<TClntAddrMgr> AddrMgr,
		    int iface, SmartPtr<TIPv6Addr> addr,
		    TContainer< SmartPtr< TAddrIA> > IALst);

    TClntMsgDecline(SmartPtr<TClntIfaceMgr> IfaceMgr,
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr> CfgMgr,
		    SmartPtr<TClntAddrMgr> AddrMgr,
		    int iface, SmartPtr<TIPv6Addr> addr, 
		    char* buf, int bufSize);
    bool  check();
    
    void  answer(SmartPtr<TClntMsg> Rep);
    void  doDuties();
    string getName();
    ~TClntMsgDecline();
 private:
};

#endif
