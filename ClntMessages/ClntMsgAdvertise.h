/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgAdvertise.h,v 1.6 2008-08-29 00:07:28 thomson Exp $
 *
 */

#ifndef SRVMSGADVERTISE_H
#define SRVMSGADVERTISE_H

#include "ClntMsg.h"

class TClntMsgAdvertise : public TClntMsg
{
  public:
    /* TClntMsgAdvertise(int iface, SPtr<TIPv6Addr> addr); */
    
    TClntMsgAdvertise(int iface, SPtr<TIPv6Addr> addr, 
                      char* buf, int bufSize);
    
    // returns preference value (default value is 0)
    int getPreference();
    bool check();
    void answer(SPtr<TClntMsg> Rep);
    void doDuties();
    string getName();
    string getInfo();
    ~TClntMsgAdvertise();
};

#endif 
