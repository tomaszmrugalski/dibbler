/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgRebind.h,v 1.5 2008-08-29 00:07:28 thomson Exp $
 *
 */

class TClntMsgRebind;
#ifndef CLNTMSGREBIND_H
#define CLNTMSGREBIND_H

#include "ClntMsg.h"
#include "ClntOptIA_NA.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerUnicast.h"

class TClntMsgRebind : public TClntMsg
{
  public:
    TClntMsgRebind(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		   SmartPtr<TClntTransMgr> TransMgr, 
		   SmartPtr<TClntCfgMgr> CfgMgr, 
		   SmartPtr<TClntAddrMgr> AddrMgr,
		   TContainer<SmartPtr<TOpt> > ptrOpts, int iface);
    
    void answer(SmartPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRebind();
 private:
    void updateIA(SmartPtr <TClntOptIA_NA> ptrOptIA,
		  SmartPtr<TClntOptServerIdentifier> optSrvDUID, 
		  SmartPtr<TClntOptServerUnicast> optUnicast);
    void releaseIA(int IAID);
    void releasePD(int IAID);


};

#endif /* REBIND_H_HEADER_INCLUDED_C1126D16 */
