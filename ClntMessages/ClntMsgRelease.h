/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRelease.h,v 1.4 2006-03-23 00:12:09 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/01/08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

class TClntMsgRelease;
#ifndef CLNTMSGRELEASE_H
#define CLNTMSGRELEASE_H
#include "ClntMsg.h"

class TClntMsgRelease : public TClntMsg
{
  public:
    TClntMsgRelease(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   ConfMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
		    int iface,  SmartPtr<TIPv6Addr> addr=NULL);
    
    TClntMsgRelease(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   CfgMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
		    int iface, SmartPtr<TIPv6Addr> addr,
		    List(TAddrIA) iaLst, 
		    SmartPtr<TAddrIA> ta);

    void answer(SmartPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRelease();
};
#endif /* CLNTMSGRELEASE_H */
