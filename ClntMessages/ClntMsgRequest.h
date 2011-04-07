/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 */

class TClntIfaceMgr;
#ifndef CLNTMSGREQUEST_H
#define CLNTMSGREQUEST_H

#include "SmartPtr.h"
#include "ClntMsg.h"

class TClntMsgRequest : public TClntMsg
{
  public:
    TClntMsgRequest(TOptList opts, int iface);
    TClntMsgRequest(List(TAddrIA) requestIALst,
                    SPtr<TDUID> srvDUID,
                    int iface);
    
    void answer(SPtr<TClntMsg> msg);
    void doDuties();
    bool check();
    string getName();
    ~TClntMsgRequest();
  private:
    void setState(TOptList opts, EState state);
    void copyAddrsFromAdvertise(SPtr<TClntMsg> adv);
    void copyPrefixesFromAdvertise(SPtr<TClntMsg> adv);
};

#endif /* CLNTMSGREQUEST_H */
