/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TClntIfaceMgr;
#ifndef CLNTMSGINFREQUEST_H
#define CLNTMSGINFREQUEST_H

#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntCfgMgr.h"

class TClntMsgInfRequest : public TClntMsg
{
  public:
    TClntMsgInfRequest(TOptList ReqOpts, int iface);
    TClntMsgInfRequest(SPtr<TClntCfgIface> iface);
    
    void answer(SPtr<TClntMsg> msg);
    void doDuties();    
    bool check();
    string getName();
    ~TClntMsgInfRequest();
};



#endif 
