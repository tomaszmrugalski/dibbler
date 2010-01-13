/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgInfRequest.h,v 1.4 2008-08-29 00:07:28 thomson Exp $
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
    TClntMsgInfRequest(SPtr<TClntIfaceMgr> IfMgr, 
		       SPtr<TClntTransMgr> TransMgr,
		       SPtr<TClntCfgMgr>   ConfMgr, 
		       SPtr<TClntAddrMgr>  AddrMgr, 
		       TContainer< SPtr<TOpt> > ReqOpts,
		       int iface);
    
    TClntMsgInfRequest(SPtr<TClntIfaceMgr> IfMgr, 
		       SPtr<TClntTransMgr> TransMgr,
		       SPtr<TClntCfgMgr>   ConfMgr, 
		       SPtr<TClntAddrMgr>  AddrMgr, 
		       SPtr<TClntCfgIface> iface);
    
    void answer(SPtr<TClntMsg> msg);
    void doDuties();    
    bool check();
    string getName();
    ~TClntMsgInfRequest();

  private:
    SPtr<TClntAddrMgr> AddrMgr;
};



#endif 
