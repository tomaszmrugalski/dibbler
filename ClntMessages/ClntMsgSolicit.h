/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgSolicit.h,v 1.6 2007-01-27 17:12:24 thomson Exp $
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
		    List(TClntCfgIA) iaLst, 
		    SPtr<TClntCfgTA> ta,
		    List(TClntCfgPD) pdLst,
		    bool rapid=false);

    void answer(SmartPtr<TClntMsg> msg);

    void doDuties();
    bool shallRejectAnswer(SmartPtr<TClntMsg> msg);
    void sortAnswers();
    string getName();
    bool check();
    ~TClntMsgSolicit();

 private:
    // method returns max. preference value of received ADVERTISE messages
    int getMaxPreference();
};
#endif 
