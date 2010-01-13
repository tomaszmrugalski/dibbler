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
    TClntMsgRebind(SPtr<TClntIfaceMgr> IfaceMgr, 
		   SPtr<TClntTransMgr> TransMgr, 
		   SPtr<TClntCfgMgr> CfgMgr, 
		   SPtr<TClntAddrMgr> AddrMgr,
		   TContainer<SPtr<TOpt> > ptrOpts, int iface);
    
    void answer(SPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRebind();
 private:
    void updateIA(SPtr <TClntOptIA_NA> ptrOptIA,
		  SPtr<TClntOptServerIdentifier> optSrvDUID, 
		  SPtr<TClntOptServerUnicast> optUnicast);
    void releaseIA(int IAID);
    void releasePD(int IAID);


};

#endif /* REBIND_H_HEADER_INCLUDED_C1126D16 */
