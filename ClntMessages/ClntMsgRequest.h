#ifndef REQUEST_H_HEADER_INCLUDED_C1127952
#define REQUEST_H_HEADER_INCLUDED_C1127952

#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgMgr.h"

class TClntIfaceMgr;

//##ModelId=3EC75CE4004D
class TClntMsgRequest : public TClntMsg
{
  public:
    //## Po wybraniu pierwszego serwera z listy serwerów, tworzona jest
    //## wiadomoœæ REQUEST.
    TClntMsgRequest(SmartPtr<TClntIfaceMgr> IfMgr, 
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   ConfMgr, 
		    SmartPtr<TClntAddrMgr>  AddrMgr, 
		    TContainer< SmartPtr<TOpt> > opts, 
		    TContainer< SmartPtr<TMsg> > advs,
		    int iface);
    TClntMsgRequest(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr>   CfgMgr, 
				 SmartPtr<TClntAddrMgr> AddrMgr, 
                 TContainer<SmartPtr<TAddrIA> > requestIALst,
                 SmartPtr<TDUID> srvDUID,
				 int iface);

	
    //## O ile odpowiedŸ zawiera IA:
    //##   1. Tworzymy nowa Transakcjê VerifyIA na podstawie otryzmanego IA w
    //## odpowiedzi
    //##   2. informuujew IfaceMgr'a ze ma przypisac otrzymany adres
    //##   3. Ustawia IsDone na true.
    //## Jeœli nie zawiera IA wówczas Request powinien rozpocz¹æ now¹
    //## transakcjê dla kolejnego Advertise, 
    //## Jeœli nie ma ju¿ Advertisow, dla wszystkich IA, które probowano
    //## skonfigurowaæ
    void answer(SmartPtr<TMsg> msg);

    //##ModelId=3EC76E1B01BB
    //##Documentation
    //## 1. wysy³a wiadomoœæ
    //## 2. retransmituje wiadomoœæ
    //## 3. w przypadku przekroczenia czasu timeoutów, zwalnia pamiêæ
    //## pierwszego serwera na liœcie, i rozpoczyna transmisjê do nastêpnego
    //## 4. je¿eli lista serwerów jest pusta, ustawia IsDone na true i zwraca
    //## FATAL.
    //## 5. Jezeli otrzymana zostanie odpowiedz wywo³uje AddrMgr i dodaje
    //## wszystkie IA dla których uda³o siê uzyskaæ odpowiedzi. (!UWAGA!
    //## ADVERTISE musi ³¹czyæ IA, o które siê ubiegamy z IA z ConfMgr)
    //## 
    void doDuties();

    //##ModelId=3EC95E3601F7
    //##Documentation
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();

    ~TClntMsgRequest();


  private:
    //##ModelId=3EC94FCF023B
    SmartPtr<TClntAddrMgr> AddrMgr;

	//##ModelId=3EFF0A95005D
    TContainer< SmartPtr<TMsg> > BackupSrvLst;

};



#endif /* REQUEST_H_HEADER_INCLUDED_C1127952 */
