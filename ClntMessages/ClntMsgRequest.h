/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRequest.h,v 1.2 2004-06-20 17:51:48 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

class TClntIfaceMgr;
#ifndef CLNTMSGREQUEST_H
#define CLNTMSGREQUEST_H

#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgMgr.h"

class TClntMsgRequest : public TClntMsg
{
  public:
    TClntMsgRequest(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   ConfMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
		    TContainer< SmartPtr<TOpt> > opts, 
		    TContainer< SmartPtr<TMsg> > advs,
		    int iface);
    TClntMsgRequest(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr>   CfgMgr, 
				 SmartPtr<TClntAddrMgr> AddrMgr, 
                 TContainer<SmartPtr<TAddrIA> > requestIALst,
                 SmartPtr<TDUID> srvDUID,
				 int iface);

    void answer(SmartPtr<TMsg> msg);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRequest();
  private:
    SmartPtr<TClntAddrMgr> AddrMgr;
    TContainer< SmartPtr<TMsg> > BackupSrvLst;

};

#endif /* CLNTMSGREQUEST_H */
