/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRenew.h,v 1.6 2007-01-07 23:31:00 thomson Exp $
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
		  List(TAddrIA) IALst, List(TAddrIA) PDLst);

    void answer(SmartPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRenew();
    void updateIA(SmartPtr <TClntOptIA_NA> ptrOptIA);
    void releaseIA(long IAID);

 private:
};

#endif 
