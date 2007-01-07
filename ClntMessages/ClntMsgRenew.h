/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRenew.h,v 1.5 2007-01-07 20:18:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005-01-08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.3  2004/09/07 22:02:32  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
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
		  List(TAddrIA) ptrLstIA);

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
