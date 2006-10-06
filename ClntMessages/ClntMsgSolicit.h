/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgSolicit.h,v 1.5 2006-10-06 00:43:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005-01-08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.3  2004/09/07 22:02:33  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
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
    
    void answer(SmartPtr<TClntMsg> msg);
    void replyReceived(SmartPtr<TClntMsg> msg);

    void doDuties();
    bool shallRejectAnswer(SmartPtr<TClntMsg> msg);
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
