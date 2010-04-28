/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TClntMsgDecline;
#ifndef CLNTMSGDECLINE_H
#define CLNTMSGDECLINE_H
#include "ClntMsg.h"

class TClntMsgDecline : public TClntMsg
{
  public:
  TClntMsgDecline(int iface, SPtr<TIPv6Addr> addr, List(TAddrIA) IALst);
  
    bool  check();
    
    void  answer(SPtr<TClntMsg> Rep);
    void  doDuties();
    string getName();
    ~TClntMsgDecline();
 private:
};

#endif
