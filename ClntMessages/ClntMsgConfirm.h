/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgConfirm.h,v 1.4 2008-08-29 00:07:28 thomson Exp $
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

class TClntMsgConfirm;    

#ifndef CLNTMSGCONFIRM_H
#define CLNTMSGCONFIRM_H
#include "ClntMsg.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "ClntCfgIA.h"

class TClntMsgConfirm : public TClntMsg
{
public:

   TClntMsgConfirm(SmartPtr<TClntIfaceMgr> IfaceMgr, 
	SmartPtr<TClntTransMgr> TransMgr, 
	SmartPtr<TClntCfgMgr>	CfgMgr, 
	SmartPtr<TClntAddrMgr> AddrMgr,
	unsigned int iface, 
    TContainer<SmartPtr<TAddrIA> > iaLst);
    bool check();
    void answer(SmartPtr<TClntMsg> Rep);
    void doDuties();
    unsigned long getTimeout();
    string getName();

    void addrsAccepted();
    void addrsRejected();
    ~TClntMsgConfirm();

};
#endif /* CLNTMSGCONFIRM_H */
