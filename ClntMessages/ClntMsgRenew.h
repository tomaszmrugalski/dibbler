#ifndef RENEW_H_HEADER_INCLUDED_C1120438
#define RENEW_H_HEADER_INCLUDED_C1120438
#include "ClntMsg.h"
#include "ClntOptIA_NA.h"

class TClntMsgRenew : public TClntMsg
{
  public:
    TClntMsgRenew(SmartPtr<TClntIfaceMgr> IfaceMgr,
		  SmartPtr<TClntTransMgr> TransMgr,
		  SmartPtr<TClntCfgMgr> CfgMgr,
		  SmartPtr<TClntAddrMgr> AddrMgr,
		  TContainer<SmartPtr<TAddrIA> > ptrLstIA);

    /*TClntMsgRenew(SmartPtr<TClntIfaceMgr> IfaceMgr,
		  SmartPtr<TClntTransMgr> TransMgr,
		  SmartPtr<TClntCfgMgr> CfgMgr,
		  SmartPtr<TClntAddrMgr> AddrMgr,
		  SmartPtr<TAddrIA> ptrIA);*/


    //## Odpowiada za reakcjê na otrzymanie wiadomoœci. W prztpadku zakoñczenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //## Funkcja odpowiada za transmisjê i retransmisjê danej wiadomoœci z
    //## uwzglednienirem sta³ych czasowych.
    void doDuties();

    bool check();
    ~TClntMsgRenew();

    void updateIA(SmartPtr <TClntOptIA_NA> ptrOptIA);
    void releaseIA(long IAID);

 private:
    //SmartPtr<TAddrIA> AddrIA;
    //TContainer<SmartPtr<TClntOpt> > IALst;
};

#endif 
