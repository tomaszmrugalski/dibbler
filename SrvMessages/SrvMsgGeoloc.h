/* 
 * File:   SrvMsgGeoloc.h
 * Author: Michal Golon
 *
 */
class TSrvMsgGeoloc;
#ifndef SRVMSGGEOLOC_H
#define	SRVMSGGEOLOC_H
#include "SrvMsg.h"
#include "SmartPtr.h"

class TSrvMsgGeoloc : public TSrvMsg
{
public:
    TSrvMsgGeoloc(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSzie);
    
    void doDuties();
    string getName();
    unsigned long getTimeout();
    bool check();
    
    ~TSrvMsgGeoloc();
private:

};

#endif	/* SRVMSGGEOLOC_H */

