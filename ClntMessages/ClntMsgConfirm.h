/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgConfirm.h,v 1.2 2004-06-20 17:51:48 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    void answer(SmartPtr<TMsg> Rep);
    void doDuties();
    unsigned long getTimeout();
    string getName();

    void addrsAccepted();
    void addrsRejected();
    ~TClntMsgConfirm();

};
#endif /* CLNTMSGCONFIRM_H */
