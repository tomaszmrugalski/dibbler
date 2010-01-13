/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgRelease.h,v 1.6 2008-08-29 00:07:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2006-10-06 00:43:28  thomson
 * Initial PD support.
 *
 * Revision 1.4  2006-03-23 00:12:09  thomson
 * TA support on the client side finished.
 *
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
    TClntMsgRelease(SPtr<TClntIfaceMgr> IfMgr, 
		    SPtr<TClntTransMgr> TransMgr,
		    SPtr<TClntCfgMgr>   ConfMgr, 
		    SPtr<TClntAddrMgr>  AddrMgr, 
		    int iface,  SPtr<TIPv6Addr> addr=NULL);
    
    TClntMsgRelease(SPtr<TClntIfaceMgr> IfMgr, 
		    SPtr<TClntTransMgr> TransMgr,
		    SPtr<TClntCfgMgr>   CfgMgr, 
		    SPtr<TClntAddrMgr>  AddrMgr, 
		    int iface, SPtr<TIPv6Addr> addr,
		    List(TAddrIA) iaLst, 
		    SPtr<TAddrIA> ta,
		    List(TAddrIA) pdLst);

    void answer(SPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRelease();
};
#endif /* CLNTMSGRELEASE_H */
