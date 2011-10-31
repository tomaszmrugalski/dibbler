/* 
 * File:   ClntMsgGeoloc.h
 * Author: Michal Golon
 *
 */
class TClntIfaceMgr;

#ifndef CLNTMSGGEOLOC_H
#define	CLNTMSGGEOLOC_H

#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntCfgMgr.h"

class TClntMsgGeoloc : public TClntMsg
{
public:
    TClntMsgGeoloc(int iface, SPtr<TIPv6Addr> addr, List(string) coordinates);
    
    void answer(SPtr<TClntMsg> msg);
    void doDuties();    
    bool check();
    string getName();
    
    ~TClntMsgGeoloc();
private:

};

#endif	/* CLNTMSGGEOLOC_H */

