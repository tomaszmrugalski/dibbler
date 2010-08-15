/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgSolicit.h,v 1.7 2008-08-29 00:07:28 thomson Exp $
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
  TClntMsgSolicit(int iface, SPtr<TIPv6Addr> addr,
                  List(TClntCfgIA) iaLst, SPtr<TClntCfgTA> ta,
                  List(TClntCfgPD) pdLst, bool rapid=false,
		  bool remoteAutoconf = false);

    void answer(SPtr<TClntMsg> msg);

    void doDuties();
    bool shallRejectAnswer(SPtr<TClntMsg> msg);
    void sortAnswers();
    string getName();
    bool check();
    ~TClntMsgSolicit();

 private:
    // method returns max. preference value of received ADVERTISE messages
    int getMaxPreference();
};
#endif 
