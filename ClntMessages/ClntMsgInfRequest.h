/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgInfRequest.h,v 1.3 2005-01-08 16:52:03 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

class TClntIfaceMgr;
#ifndef CLNTMSGINFREQUEST_H
#define CLNTMSGINFREQUEST_H

#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgMgr.h"

class TClntMsgInfRequest : public TClntMsg
{
  public:
    TClntMsgInfRequest(SmartPtr<TClntIfaceMgr> IfMgr, 
		       SmartPtr<TClntTransMgr> TransMgr,
		       SmartPtr<TClntCfgMgr>   ConfMgr, 
		       SmartPtr<TClntAddrMgr>  AddrMgr, 
		       TContainer< SmartPtr<TOpt> > ReqOpts,
		       int iface);
    
    TClntMsgInfRequest(SmartPtr<TClntIfaceMgr> IfMgr, 
		       SmartPtr<TClntTransMgr> TransMgr,
		       SmartPtr<TClntCfgMgr>   ConfMgr, 
		       SmartPtr<TClntAddrMgr>  AddrMgr, 
		       SmartPtr<TClntCfgIface> iface);
    
    void answer(SmartPtr<TClntMsg> msg);
    void doDuties();    
    bool check();
    string getName();
    ~TClntMsgInfRequest();

  private:
    SmartPtr<TClntAddrMgr> AddrMgr;
};



#endif 
