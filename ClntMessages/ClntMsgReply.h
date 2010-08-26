/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TClntMsgReply;
#ifndef CLNTMSGREPLY_H
#define CLNTMSGREPLY_H
#include "ClntMsg.h"

class TClntMsgReply : public TClntMsg
{
  public:
    TClntMsgReply(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize);

    SPtr<TIPv6Addr> getFirstAddr();
    void answer(SPtr<TClntMsg> Rep);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgReply();
};

#endif /* CLNTMSGREPLY_H */
