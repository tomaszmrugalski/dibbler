/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgSolicit.h,v 1.3 2004-09-07 22:02:33 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

class TClntIfaceMgr;

#ifndef CLNTMSGSOLICIT_H
#define CLNTMSGSOLICIT_H
#include "ClntMsg.h"
#include "ClntCfgMgr.h"
#include "ClntCfgIA.h"

class TClntMsgSolicit : public TClntMsg
{
public:
    TClntMsgSolicit(SmartPtr<TClntIfaceMgr> IfaceMgr,
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   CfgMgr,
		    SmartPtr<TClntAddrMgr>  AddrMgr,
		    int iface,
		    SmartPtr<TIPv6Addr> addr,
		    TContainer< SmartPtr<TClntCfgIA> > IAs, bool rapid=false);
    
    void answer(SmartPtr<TMsg> msg);
    void replyReceived(SmartPtr<TMsg> msg);

    void doDuties();
    bool shallRejectAnswer(SmartPtr<TMsg> msg);
    void sortAnswers();
    string getName();
    bool check();
    ~TClntMsgSolicit();

 private:
    List(TMsg) AnswersLst;

    // method returns max. preference value of received ADVERTISE messages
    int getMaxPreference();
};
#endif 
