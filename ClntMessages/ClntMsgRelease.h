/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TClntMsgRelease;
#ifndef CLNTMSGRELEASE_H
#define CLNTMSGRELEASE_H
#include "ClntMsg.h"

class TClntMsgRelease : public TClntMsg
{
  public:
    /* TClntMsgRelease(int iface, SPtr<TIPv6Addr> addr=NULL); */
    
    TClntMsgRelease(int iface, SPtr<TIPv6Addr> addr,
                    List(TAddrIA) iaLst, 
                    SPtr<TAddrIA> ta,
                    List(TAddrIA) pdLst);

    void answer(SPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRelease();
};
#endif /* CLNTMSGRELEASE_H */
