/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRenew.h,v 1.2 2004-06-20 17:51:48 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTMSGRENEW_H
#define CLNTMSGRENEW_H
#include "ClntMsg.h"
#include "ClntOptIA_NA.h"

class TClntMsgRenew : public TClntMsg
{
  public:
    TClntMsgRenew(SmartPtr<TClntIfaceMgr> IfaceMgr,
		  SmartPtr<TClntTransMgr> TransMgr,
		  SmartPtr<TClntCfgMgr> CfgMgr,
		  SmartPtr<TClntAddrMgr> AddrMgr,
		  TContainer<SmartPtr<TAddrIA> > ptrLstIA);

    void answer(SmartPtr<TMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRenew();
    void updateIA(SmartPtr <TClntOptIA_NA> ptrOptIA);
    void releaseIA(long IAID);

 private:
};

#endif 
