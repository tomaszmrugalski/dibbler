class TClntMsgRelease;
#ifndef RELEASE_H_HEADER_INCLUDED_C1123867
#define RELEASE_H_HEADER_INCLUDED_C1123867

#include "ClntMsg.h"

//##ModelId=3EC75CE40043
//##Documentation
//## Transakcja Release tworzona jest w przypadku:
//## 1. zatrzymywanie us³ugi (Tworzona w metodzie stop())
class TClntMsgRelease : public TClntMsg
{
  public:
    //##ModelId=3EC8AA0D017D
    TClntMsgRelease(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   ConfMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
		    int iface,  SmartPtr<TIPv6Addr> addr=NULL);
    
    TClntMsgRelease(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   CfgMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
		    int iface, SmartPtr<TIPv6Addr> addr,
		    TContainer<SmartPtr <TAddrIA> > iaLst);

    //##ModelId=3ECA86C001AD
    //##Documentation
    //## Odpowiada za reakcjê na otrzymanie wiadomoœci. W prztpadku zakoñczenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //##ModelId=3ECA86C001CB
    //##Documentation
    //## Funkcja odpowiada za transmisjê i retransmisjê danej wiadomoœci z
    //## uwzglednienirem sta³ych czasowych.
    void doDuties();


    //##ModelId=3ECA86C001FD
    //##Documentation
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();

    //##ModelId=3EC8AA180101
    ~TClntMsgRelease();
};
#endif /* RELEASE_H_HEADER_INCLUDED_C1123867 */
