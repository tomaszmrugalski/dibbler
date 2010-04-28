/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TClntMsgConfirm;    

#ifndef CLNTMSGCONFIRM_H
#define CLNTMSGCONFIRM_H
#include "ClntMsg.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "ClntCfgIA.h"

class TClntMsgConfirm : public TClntMsg
{
public:
    TClntMsgConfirm(unsigned int iface, List(TAddrIA) iaLst);
    bool check();
    void answer(SPtr<TClntMsg> Rep);
    void doDuties();
    unsigned long getTimeout();
    string getName();

    void addrsAccepted();
    void addrsRejected();
    ~TClntMsgConfirm();

};
#endif /* CLNTMSGCONFIRM_H */
